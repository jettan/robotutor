#include <functional>
#include <iostream>

#include <boost/asio/io_service.hpp>

#include "behavior_engine.hpp"

namespace robotutor {
	
	/// Construct a behavior job.
	/**
	 * \param name The name of the behavior to execute.
	 * \param on_done Callback to be invoked when the job is finished.
	 */
	BehaviorJob::BehaviorJob(std::string const & name, EventHandler on_done, EventHandler on_start) :
		name_(name),
		on_start_(on_done),
		on_done_(on_done),
		id_(0) {}
	
	/// Construct the behaviour engine.
	/**
	 * \param parent The parent script engine.
	 * \param ios The IO service to use.
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	BehaviorEngine::BehaviorEngine(ScriptEngine & parent, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker) :
		parent_(parent),
		ios_(ios),
		bm_(broker) {}
	
	
	/// Queue a job for execution.
	/**
	 * \param job The job.
	 */
	void BehaviorEngine::enqueue(BehaviorJob const & job) {
		queue_.push_back(job);
		if (queue_.size() == 1) {
			unqueue_();
		}
	}
	
	/// Drop all queued jobs.
	/**
	 * If there is a job currently running, it will continue as normal.
	 */
	void BehaviorEngine::drop() {
		queue_.erase(queue_.begin() + 1, queue_.end());
	}
	
	/// Process the head of the queue.
	/**
	 * The job remains at the head of the queue until it finished.
	 */
	void BehaviorEngine::unqueue_() {
		BehaviorJob & job = queue_.front();
		
		// Call the start handler.
		if (job.on_start_) job.on_start_();
		
		// If the behavior can be found, execute it.
		if (bm_.isBehaviorInstalled(job.name_)) {
			std::cout << "Running behaviour: `" << job.name_ << "'." << std::endl;
			job.id_ = bm_.post.runBehavior(job.name_);
			
			// Start a new wait thread.
			if (wait_thread_.joinable()) wait_thread_.join();
			wait_thread_ = std::thread(std::bind(&BehaviorEngine::wait_, this));
		
		// Otherwise, immediately call the done handler.
		} else {
			std::cout << "Behavior not found: `" << job.name_ << "'." << std::endl;
			if (job.on_done_) job.on_done_();
		}
	}
	
	/// Called when a job finished.
	void BehaviorEngine::onJobDone_() {
		// Invoke the done handler and pop the job.
		BehaviorJob & job = queue_.front();
		if (job.on_done_) job.on_done_();
		queue_.pop_front();
		
		// Process the rest of the queue.
		if (queue_.size()) {
			unqueue_();
		// If the queue is empty, inform interested parties.
		} else {
			on_done();
		}
	}
	
	/// Wait for the current job to finish.
	void BehaviorEngine::wait_() {
		bm_.wait(queue_.front().id_, 0);
		ios_.post(std::bind(&BehaviorEngine::onJobDone_, this));
	}
}

