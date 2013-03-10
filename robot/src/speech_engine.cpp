#include <boost/asio/io_service.hpp>

#include <alcommon/albroker.h>
#include <alproxies/almemoryproxy.h>

#include "speech_engine.hpp"
#include "core_commands.hpp"


namespace robotutor {
	/// Construct the speech engine.
	SpeechEngine::SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALModule(broker, name)
	{
		setModuleDescription("RoboTutor Interrupting Speech Engine");
		
		functionName("onBookmark", getName(), "Handle bookmarks.");
		BIND_METHOD(SpeechEngine::onBookmark);
	}
	
	/// Deconstruct the speech engine.
	SpeechEngine::~SpeechEngine() {
		memory_->unsubscribeToEvent("ALTextToSpeech/CurrentBookMark", getName());
	}
	
	/// Create a speech engine.
	boost::shared_ptr<SpeechEngine> SpeechEngine::create(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, std::string const & name) {
		boost::shared_ptr<SpeechEngine> result = ALModule::createModule<SpeechEngine>(broker, name);
		result->ios_    = &ios;
		return result;
	}
	
	/// Initialize the module.
	void SpeechEngine::init() {
		memory_ = getParentBroker()->getMemoryProxy();
		tts_ = AL::ALTextToSpeechProxy(getParentBroker());
		tts_.enableNotifications();
		
		memory_->subscribeToEvent("ALTextToSpeech/CurrentBookMark", getName(), "onBookmark");
	}
	
	/// Join the background thread to ensure we can safely be destructed.
	/**
	 * Make sure that the IO service has already been stopped,
	 * or it may still process events that use the speech engine.
	 */
	void SpeechEngine::join() {
		if (wait_thread_.joinable()) wait_thread_.join();
	}
	
	/// Execute a speech command.
	/**
	 * May not be called while the engine is already executing a job.
	 * 
	 * \param command The speech command to execute.
	 * \param bookmark_handler Callback to invoke when a bookmark is encountered.
	 * \param done_handler Callback to invoke when the job is finished.
	 */
	void SpeechEngine::say(command::Speech & command, SpeechJob::BookmarkHandler bookmark_handler, SpeechJob::DoneHandler done_handler) {
		cancel();
		if (wait_thread_.joinable()) wait_thread_.join();
		
		int job_id = tts_.post.say(command.text);
		auto job = std::make_shared<SpeechJob>(&command, job_id, bookmark_handler, done_handler);
		std::cout << "Job started: " << job << " " << command.text << std::endl;
		job_ = job;
		wait_thread_ = std::thread([this, job] () {
			wait_(job);
		});
	}
	
	/// Cancel the current job.
	void SpeechEngine::cancel() {
		if (job_) {
			tts_.stop(job_->id);
			job_->on_done(true);
			job_->id = 0;
			job_ = std::shared_ptr<SpeechJob>();
		}
	}
	
	/// Called when a bookmark is encountered.
	void SpeechEngine::onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier) {
		// Post the event to the io_service.
		ios_->post([this, value] () {
			handleBookmark_(value);
		});
	}
	
	/// Wait for a job and post the event.
	/**
	 * \param job The job ID.
	 */
	void SpeechEngine::wait_(std::shared_ptr<SpeechJob> job) {
		tts_.wait(job->id, 0);
		ios_->post([this, job] () {
			handleJobDone_(job);
		});
	}
	
	/// Called when a bookmark is encountered.
	/**
	 * \param bookmark The number of the bookmark.
	 */
	void SpeechEngine::handleBookmark_(int bookmark) {
		// Bookmark 0 gets abused, so we ignore that one.
		if (bookmark > 0 && job_ && job_->id) {
			job_->on_bookmark(static_cast<unsigned int>(bookmark));
		}
	}
	
	/// Called when a job is done.
	void SpeechEngine::handleJobDone_(std::shared_ptr<SpeechJob> job) {
		std::cout << "Job finished. Current: " << job_ << ". Finished: " << job << "." << std::endl;
		// Unset the current job so that the done handler can safely start a new job.
		if (job == job_) job_ = std::shared_ptr<SpeechJob>();
		
		// If the job hasn't been cancelled, invoke the done handler.
		if (job->id) {
			job->on_done(false);
			job->id = 0;
		}
	}
	
}
