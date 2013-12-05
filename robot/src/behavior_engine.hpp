#pragma once

#include <string>
#include <deque>
#include <functional>
#include <thread>

#include <boost/signal.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <alproxies/albehaviormanagerproxy.h>

#define BEHAVIOR_TIMEOUT 10000

namespace boost {
	namespace asio {
		class io_service;
	}
}

namespace robotutor {
	
	class ScriptEngine;
	
	/// Behavior engine job.
	class BehaviorJob {
		friend class BehaviorEngine;
		
		public:
			typedef std::function<void ()> EventHandler;
			
		protected:
			/// The name of the behavior.
			std::string name_;
			
			/// Called just before the job starts.
			EventHandler on_start_;
			
			/// Called when the job is finished.
			/**
			 * The job is still in the head of the queue when this callback is invoked.
			 */
			EventHandler on_done_;
			
			/// The ID from the behavior manager proxy.
			int id_;
			
		public:
			/// Construct a behavior job.
			/**
			 * \param name The name of the behavior to execute.
			 * \param on_done Callback to be invoked when the job is finished.
			 */
			explicit BehaviorJob(std::string const & name, EventHandler on_done = nullptr, EventHandler on_start = nullptr);
	};
	
	/**
	 * Behaviour engine.
	 */
	class BehaviorEngine {
		public:
			/// Signal invoked when the engine finished all queued jobs.
			boost::signal<void ()> on_done;
			
			/// The associated script engine.
			ScriptEngine * engine;
			
		protected:
			/// IO service to perform asynchronous work.
			boost::asio::io_service & ios_;
			
			/// Job queue.
			std::deque<BehaviorJob> queue_;
			
			/// Behavior manager to use.
			AL::ALBehaviorManagerProxy bm_;
			
			/// Thread to wait for job completion.
			std::thread wait_thread_;
			
			/// Random number generator.
			boost::random::mt19937 & random_;
			
		public:
			/// Construct the behaviour engine.
			/**
			 * \param ios The IO service to use.
			 * \param broker The ALBroker to use for communicating with naoqi.
			 * \param random Random number generator to use.
			 */
			BehaviorEngine(ScriptEngine * engine, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, boost::random::mt19937 & random);
			
			/// Get the number of queued jobs.
			unsigned int queued() { return queue_.size(); }
			
			/// Queue a job for execution.
			/**
			 * \param job The job.
			 */
			void enqueue(BehaviorJob const & job);
			
			/// Queue a random behavior.
			/**
			 * \param prefix The prefix to select behaviors from.
			 */
			void enqueueRandom(std::string const & prefix);
			
			/// Queue a job for execution.
			/**
			 * \param args The arguments for the job constructor.
			 */
			template<typename... Args>
			void enqueue(Args... args) {
				enqueue(BehaviorJob(args...));
			}
			
			/// Join any background threads created by the behavior engine.
			void join();
			
			/// Drop all queued jobs.
			/**
			 * If there is a job currently running, it will continue as normal.
			 */
			void drop();
			
		protected:
			/// Process the head of the queue.
			/**
			 * The job remains at the head of the queue until it finished.
			 */
			void unqueue_();
			
			/// Called when a job finished.
			void onJobDone_();
			
			/// Wait for the current job to finish.
			void wait_();
			
	};
	
}
