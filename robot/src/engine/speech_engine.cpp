#include <boost/asio.hpp>

#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"
#include "script_engine.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name)
	{
		setModuleDescription("RoboTutor Interrupting Speech Engine");
		
		functionName("onBookmark", getName(), "Handle bookmarks.");
		BIND_METHOD(SpeechEngine::onBookmark);
		
		functionName("onTextDone", getName(), "Handle job completion.");
		BIND_METHOD(SpeechEngine::onTextDone);
	}
	
	/// Deconstruct the speech engine.
	SpeechEngine::~SpeechEngine() {
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName());
		memory_->unsubscribeToEvent("ALTextToSpeech/TextDone"             , getName());
	}
	
	/// Create a speech engine.
	boost::shared_ptr<SpeechEngine> SpeechEngine::create(ScriptEngine & parent, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, std::string const & name) {
		boost::shared_ptr<SpeechEngine> result = ALModule::createModule<SpeechEngine>(broker, name);
		result->ios_    = &ios;
		result->parent_ = &parent;
		return result;
	}
	
	/// Initialize the module.
	void SpeechEngine::init() {
		memory_ = getParentBroker()->getMemoryProxy();
		tts_ = AL::ALTextToSpeechProxy(getParentBroker());
		tts_.enableNotifications();
		
		memory_->subscribeToEvent("ALTextToSpeech/CurrentBookMark"      , getName(), "onBookmark");
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
			current_->interrupts.push_back(std::make_shared<SpeechContext>(*parent_, text, current_));
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
		
		queue_.push_back(SpeechContext(*parent_, text, nullptr));
		if (!current_) {
			current_ = &queue_.front();
			resume();
		}
	}
	
	/// Stop execution.
	void SpeechEngine::pause() {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		if (!playing_) return;
		
		playing_ = false;
		on_pause(*this);
	}
	
	/// Resume execution.
	void SpeechEngine::resume() {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		if (playing_) return;
		
		playing_ = true;
		on_resume(*this);
		
		step_();
		
		if (!current_) on_done(*this);
	}
	
	/// Reset the engine.
	void SpeechEngine::reset() {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		pause();
		current_ = nullptr;
		queue_.clear();
		on_done(*this);
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		// Bookmark 0 gets abused, so we ignore that one.
		if (value > 0) {
			current_->executeCommand(value - 1);
		}
	}
	
	/// Called when the TTS engine is done.
	void SpeechEngine::onTextDone(std::string const & eventName, bool const & value, std::string const & subsciberIdentifier) {
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		if (!tts_.isRunning(job_id_)) {
			job_id_ = 0;
			if (playing_) step_();
			if (!current_) on_done(*this);
		}
	}
	
	/// Step trough the contexts.
	void SpeechEngine::step_() {
		while (current_ && !current_->step());
		if (!current_) playing_ = false;
	}
	
	/// Say a text.
	/**
	 * \param text The text.
	 */
	void SpeechEngine::say_(std::string const & text) {
		job_id_ = tts_.post.say(text);
	}
	
	/// Execute all unexecuted commands up to the given index.
	/**
	 * \param command The index.
	 */
	void SpeechContext::executeCommand(int command) {
		for (; mark <= command; ++mark) {
			text->arguments[mark]->run(parent);
		}
	}
	
	/// Perform the next step of the context.
	/**
	 * If this method returns true, the next step should wait
	 * until the started aynchronous operation completed.
	 * 
	 * \return True if the step started an asynchronous operation.
	 */
	bool SpeechContext::step() {
		bool sentences_done = sentence >= text->sentences.size();
		std::cout
			<< "\n"
			<< "step\n"
			<< "sentence: " << sentence << "\n"
			<< "mark:" << mark << "\n"
			<< "interrupts: " << interrupts.size() << "\n"
			<< "mark target: " << text->marks[sentence] << "\n"
			<< "action: ";
		// If there's an interrupt pending, switch context.
		if (interrupts.size()) {
			std::cout << "down" << std::endl;
			parent.speech->current_ = interrupts[0].get();
			return false;
			
		// If there are commands left to execute, execute them.
		} else if (!sentences_done && mark <= text->marks[sentence]) {
			std::cout << "pre-commands" << std::endl;
			executeCommand(text->marks[sentence]);
			return false;
			
		// If there's a sentence to say, say it.
		} else if (!sentences_done) {
			std::cout << "text" << std::endl;
			parent.speech->say_(text->sentences[sentence++]);
			return true;
			
		// If there's commands left to execute after the last sentence, execute them.
		} else if (mark < text->arguments.size()) {
			std::cout << "post-commands" << std::endl;
			executeCommand(text->arguments.size() - 1);
			return false;
			
		// Otherwise we're done, and we interrupted someone.
		// That command should continue.
		} else if (interrupted) {
			std::cout << "up" << std::endl;
			interrupted->interrupts.pop_front();
			parent.speech->current_ = interrupted;
			return false;
			
		// Otherwise we're a top level command,
		// and the next command should come from the queue.
		} else  {
			parent.speech->queue_.pop_front(); // Note that this deconstructs us.
			if (parent.speech->queue_.size()) {
				std::cout << "up" << std::endl;
				parent.speech->current_ = &parent.speech->queue_.front();
			} else {
				std::cout << "done" << std::endl;
				parent.speech->current_ = nullptr;
			}
			return false;
		}
	}
	
}
