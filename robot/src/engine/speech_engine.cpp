#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name)
	{
		setModuleDescription("RoboTutor speech engine");
		
		functionName("onBookmark", getName(), "Handle bookmarks.");
		BIND_METHOD(SpeechEngine::onBookmark);
		
		functionName("onWordPos", getName(), "Handle word positions.");
		BIND_METHOD(SpeechEngine::onWordPos);
	}
	
	/// Deconstruct the speech engine.
	SpeechEngine::~SpeechEngine() {
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentBookMark"    , getName());
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentWordPosition", getName());
	}
	
	/// Initialize the naoqi module.
	void SpeechEngine::init() {
		memory_ = getParentBroker()->getMemoryProxy();
		tts_ = AL::ALTextToSpeechProxy(getParentBroker());
		tts_.enableNotifications();
		
		memory_->subscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName(), "onBookmark");
		memory_->subscribeToEvent("ALTextToSpeech/PositionOfCurrentWord", getName(), "onWordPos");
	}
	
	/// Execute a text command.
	/**
	 * The previous command is interrupted and will be resumed when this one finishes.
	 * \param text The text to execute.
	 */
	void SpeechEngine::execute(executable::Text::SharedPtr text) {
		tts_.stopAll();
		int job_id = tts_.post.say(text->text);
		stack_.push_back(SpeechContext(text, job_id));
	}
	
	/// Stop execution.
	void SpeechEngine::stop() {
		tts_.stopAll();
	}
	
	/// Resume execution.
	void SpeechEngine::resume() {
		if (stack_.size()) {
			resumeContext_(stack_.back());
		}
	}
	
	/// Reset the engine.
	void SpeechEngine::reset() {
		stop();
		stack_.clear();
	}
	
	/// Resume a speech context.
	/**
	 * \param context The context to resume.
	 */
	void SpeechEngine::resumeContext_(SpeechContext & context) {
		
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::cout << "Bookmark: " << eventName << " " << value << " " << subscriberIndentifier << "." << std::endl;
	}
	
	/// Called when word is finished.
	void SpeechEngine::onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::cout << "Word position: " << eventName << " " << value << " " << subscriberIndentifier << "." << std::endl;
	}
	
}
