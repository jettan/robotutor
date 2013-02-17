#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name)
	{
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
	
	/// Execute a text command by interrupting the current text.
	/**
	 * Note that the current text will be interrupted at the next sentence end.
	 * 
	 * \param text The text command to execute.
	 */
	void SpeechEngine::interrupt(command::Text::SharedPtr text) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		if (current_) {
			current_->interrupts.push_back(SpeechContext(text, current_));
		} else {
			enqueue(text);
		}
	}
	
	/// Execute a text command.
	/**
	 * The text command will be queued and executed when the last of the currently remaining job is done.
	 * 
	 * \param text The text command to execute.
	 */
	void SpeechEngine::enqueue(command::Text::SharedPtr text) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		queue_.push_back(SpeechContext(text, nullptr));
		if (!current_) {
			current_ = &queue_.front();
			resume();
		}
	}
	
	/// Stop execution.
	void SpeechEngine::pause() {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		playing_ = false;
	}
	
	/// Resume execution at the top of the stack.
	void SpeechEngine::resume() {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		if (current_) {
			playing_ = true;
			job_id_ = tts_.post.say(current_->currentSentence());
		} else {
			playing_ = false;
		}
	}
	
	/// Reset the engine.
	void SpeechEngine::reset() {
		pause();
		queue_.clear();
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		// Bookmark 0 gets abused, so we ignore that one.
		if (value > 0) {
			current_->text->arguments[value - 1]->run(*parent_);
		}
	}
	
	/// Called when word is finished.
	void SpeechEngine::onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
	}
	
	/// Called when the TTS engine is done.
	void SpeechEngine::onTextDone(std::string const & eventName, bool const & value, std::string const & subsciberIdentifier) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		if (!tts_.isRunning(job_id_)) {
			job_id_ = 0;
			nextSentence_();
			if (playing_) resume();
		}
	}
	
	/// Recursively update the state of the speech engine so that it's ready to say the next sentence.
	/**
	 * This method sets current_ to the correct context,
	 * and removes any finished context encoutered on the way.
	 */
	void SpeechEngine::nextSentence_() {
		++current_->sentence;
		while (true) {
			// Current command has an interrupt pending, first interrupt is next.
			if (current_->interrupts.size()) {
				current_ = &current_->interrupts.front();
				
			// Current command has no interrupt pending and it is not done, so it's next.
			} else if (!current_->done()) {
				break;
				
			// Current command is done and interrupted someone else.
			// The interrupted command is next.
			} else if (current_->interrupted) {
				current_ = current_->interrupted;
				current_->interrupts.pop_front();
				
			// Current command is a top level command.
			} else {
				queue_.pop_front();
				// There is a command left to execute, it's next.
				if (queue_.size()) {
					current_ = &queue_.front();
				// No commands left.
				} else {
					current_ = nullptr;
					break;
				}
			}
		}
	}
	
}
