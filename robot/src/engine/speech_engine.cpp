#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(ScriptEngine & script_engine, boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name),
		script_engine_(script_engine)
	{
		setModuleDescription("RoboTutor Interrupting Speech Engine");
		
		functionName("onBookmark", getName(), "Handle bookmarks.");
		BIND_METHOD(SpeechEngine::onBookmark);
		
		functionName("onWordPos", getName(), "Handle word positions.");
		BIND_METHOD(SpeechEngine::onWordPos);
		
		memory_ = getParentBroker()->getMemoryProxy();
		tts_ = AL::ALTextToSpeechProxy(getParentBroker());
		tts_.enableNotifications();
		
		memory_->subscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName(), "onBookmark");
		memory_->subscribeToEvent("ALTextToSpeech/PositionOfCurrentWord", getName(), "onWordPos");
	}
	
	/// Deconstruct the speech engine.
	SpeechEngine::~SpeechEngine() {
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentBookMark"    , getName());
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentWordPosition", getName());
	}
	
	/// Execute a text command.
	/**
	 * The previous command is interrupted and will be resumed when this one finishes.
	 * \param text The text to execute.
	 */
	void SpeechEngine::executeCommand(command::Text::SharedPtr text) {
		tts_.stopAll();
		stack.push_back(SpeechContext(text));
		tts_.post.say(text->text);
	}
	
	/// Stop execution.
	void SpeechEngine::stopCommand() {
		tts_.stopAll();
	}
	
	/// Resume execution at the top of the stack.
	void SpeechEngine::resumeCommand() {
		if (stack.size()) {
			tts_.stopAll();
			SpeechContext & context = stack.back();
			context.start = context.last_word;
			tts_.post.say(context.text->text.substr(context.start));
		}
	}
	
	/// Reset the engine.
	void SpeechEngine::reset() {
		stopCommand();
		stack.clear();
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		// Bookmark 0 indicates the current job is done.
		if (value == 0) {
			stack.pop_back();
			resumeCommand();
		// Other bookmarks are command callbacks.
		} else {
			stack.back().text->arguments[value - 1]->run(script_engine_); // Bookmarks are 1 based.
		}
	}
	
	/// Called when word is finished.
	void SpeechEngine::onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		// Remember the last spoken word so we can pick up if we get interrupted.
		auto context = stack.back();
		context.last_word = context.start + value;
	}
	
}
