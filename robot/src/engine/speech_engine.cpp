#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name)
	{
		playing_ = false;
		job_id_  = 0;
		
		setModuleDescription("RoboTutor Interrupting Speech Engine");
		
		functionName("onBookmark", getName(), "Handle bookmarks.");
		BIND_METHOD(SpeechEngine::onBookmark);
		
		functionName("onWordPos", getName(), "Handle word positions.");
		BIND_METHOD(SpeechEngine::onWordPos);
		
		functionName("onTextDone", getName(), "Handle job completion.");
		BIND_METHOD(SpeechEngine::onTextDone);
	}
	
	/// Deconstruct the speech engine.
	SpeechEngine::~SpeechEngine() {
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName());
		memory_->unsubscribeToEvent("ALTextToSpeech/PositionOfCurrentWord", getName());
		memory_->unsubscribeToEvent("ALTextToSpeech/TextDone"             , getName());
	}
	
	/// Create a speech engine.
	boost::shared_ptr<SpeechEngine> SpeechEngine::create(ScriptEngine & parent, boost::shared_ptr<AL::ALBroker> broker, std::string const & name) {
		boost::shared_ptr<SpeechEngine> result = ALModule::createModule<SpeechEngine>(broker, name);
		result->parent_ = &parent;
		return result;
	}
	
	/// Initialize the module.
	void SpeechEngine::init() {
		memory_ = getParentBroker()->getMemoryProxy();
		tts_ = AL::ALTextToSpeechProxy(getParentBroker());
		tts_.enableNotifications();
		
		memory_->subscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName(), "onBookmark");
		memory_->subscribeToEvent("ALTextToSpeech/PositionOfCurrentWord", getName(), "onWordPos");
		memory_->subscribeToEvent("ALTextToSpeech/TextDone"             , getName(), "onTextDone");
		
	}
	
	/// Execute a text command.
	/**
	 * The previous command is interrupted and will be resumed when this one finishes.
	 * \param text The text to execute.
	 */
	void SpeechEngine::executeCommand(std::shared_ptr<command::Text const> text) {
		tts_.stopAll();
		stack.push_back(SpeechContext(text));
		job_id_  = tts_.post.say(text->text);
		playing_ = true;
	}
	
	/// Stop execution.
	void SpeechEngine::stopCommand() {
		tts_.stopAll();
		playing_ = false;
	}
	
	/// Resume execution at the top of the stack.
	void SpeechEngine::resumeCommand() {
		if (stack.size()) {
			tts_.stopAll();
			SpeechContext & context = stack.back();
			context.start = context.last_word;
			job_id_       = tts_.post.say(context.text->text.substr(context.start));
		}
	}
	
	/// Reset the engine.
	void SpeechEngine::reset() {
		stopCommand();
		stack.clear();
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		
		// Bookmark 0 gets abused, so we ignore that one.
		if (value > 0) {
			stack.back().text->arguments[value - 1]->run(*parent_);
		}
	}
	
	/// Called when word is finished.
	void SpeechEngine::onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		
		// Remember the last spoken word so we can pick up if we get interrupted.
		if (stack.size()) {
			auto context = stack.back();
			context.last_word = context.start + value;
		}
	}
	
	/// Called when the TTS engine is done.
	void SpeechEngine::onTextDone(std::string const & eventName, bool const & value, std::string const & subsciberIdentifier) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		
		// We get value = 0 when we stop a job ourselves.
		if (!tts_.isRunning(job_id_)) {
			stack.pop_back();
			if (playing_ && stack.size()) {
				resumeCommand();
			} else {
				playing_ = false;
			}
		}
	}
	
}
