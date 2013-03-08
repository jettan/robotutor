#pragma once

#include <string>
#include <deque>
#include <functional>
#include <memory>
#include <thread>

#include <boost/signal.hpp>

#include <alcommon/almodule.h>
#include <alproxies/altexttospeechproxy.h>

#include "../command/speech_commands.hpp"


namespace boost {
	namespace asio {
		class io_service;
	}
}

namespace AL {
	class ALBroker;
	class ALMemoryProxy;
}


namespace robotutor {
	
	class ScriptEngine;
	
	/// Struct representing a speech job.
	struct SpeechJob {
		typedef std::function<void (unsigned int bookmark)> BookmarkHandler;
		typedef std::function<void (bool interrupted)>         DoneHandler;
		
		/// The speech command that initiated the job.
		command::Speech * command;
		
		/// The job ID from the TTS proxy.
		int id;
		
		/// Callback for bookmarks.
		BookmarkHandler on_bookmark;
		
		/// Callback for when the job is done or interrupted.
		DoneHandler on_done;
		
		/// Construct a speech job.
		/**
		 * \param command The speech command that initiated the job.
		 * \param id The job ID from the TTS proxy.
		 * \param on_bookmark Callback for bookmarks.
		 * \param on_done Callback for when the job is done or interrupted.
		 */
		SpeechJob(command::Speech * command, int id, BookmarkHandler on_bookmark, DoneHandler on_done) :
			command(command),
			id(id),
			on_bookmark(on_bookmark),
			on_done(on_done) {}
	};
	
	/// Speech engine to execute command::Text.
	class SpeechEngine : public AL::ALModule {
		
		protected:
			/// IO service to perform asynchronous work.
			boost::asio::io_service * ios_;
			
			/// The currently running job.
			std::shared_ptr<SpeechJob> job_;
			
			/// Memory proxy to receive callbacks.
			boost::shared_ptr<AL::ALMemoryProxy> memory_;
			
			/// TTS proxy to do the actual synthesizing.
			AL::ALTextToSpeechProxy tts_;
			
			/// Thread to wait for job completion.
			std::thread wait_thread_;
			
		public:
			/// Construct the speech engine.
			/**
			 * \param broker The broker to use.
			 * \param name The name of the module.
			 */
			SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstruct the speech engine.
			virtual ~SpeechEngine();
			
			/// Get the currently running job.
			/**
			 * \param return The currently running job.
			 */
			std::shared_ptr<SpeechJob> job() { return job_; }
			
			/// Create a speech engine.
			/**
			 * \param ios The IO service to use.
			 * \param broker The broker to use.
			 * \param name The name of the module.
			 */
			static boost::shared_ptr<SpeechEngine> create(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Initialize the module.
			void init();
			
			/// Join the background thread to ensure we can safely be destructed.
			/**
			 * Make sure that the IO service has already been stopped,
			 * or it may still process events that use the speech engine.
			 */
			void join();
			
			/// Execute a speech command.
			/**
			 * May not be called while the engine is already executing a job.
			 * 
			 * \param command The speech command to execute.
			 * \param bookmark_handler Callback to invoke when a bookmark is encountered.
			 * \param done_handler Callback to invoke when the job is finished.
			 */
			void say(command::Speech & command, SpeechJob::BookmarkHandler bookmark_handler, SpeechJob::DoneHandler done_handler);
			
			/// Cancel the current job.
			void cancel();
			
			/// Called when a bookmark is encountered.
			void onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
		protected:
			/// Wait for a job and post the event.
			/**
			 * \param job The job ID.
			 */
			void wait_(std::shared_ptr<SpeechJob> job);
			
			/// Handle a bookmark.
			/**
			 * \param bookmark The number of the bookmark.
			 */
			void handleBookmark_(int bookmark);
			
			/// Handle the text done event.
			void handleJobDone_(std::shared_ptr<SpeechJob> job);
			
	};
}
