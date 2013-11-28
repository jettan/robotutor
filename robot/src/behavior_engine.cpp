#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/asio/io_service.hpp>

#include "behavior_engine.hpp"
#include "script_engine.hpp"
#include "robotutor_protocol.hpp"


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
	 * \param ios The IO service to use.
	 * \param broker The ALBroker to use for communicating with naoqi.
	 * \param random Random number generator to use.
	 */
	BehaviorEngine::BehaviorEngine(ScriptEngine * engine, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, boost::random::mt19937 & random) :
		engine(engine),
		ios_(ios),
		bm_(broker),
		random_(random) {}
	
	
	/// Queue a job for execution.
	/**
	 * \param job The job.
	 */
	void BehaviorEngine::enqueue(BehaviorJob const & job) {
		queue_.push_back(job);
		//std::cout << "Enqueing job " << job.name_ << ", size " << queue_.size() << std::endl;
		if (queue_.size() == 1) {
			unqueue_();
		}
	}
	
	/// Queue a random behavior.
	/**
	 * \param prefix The prefix to select behaviors from.
	 */
	void BehaviorEngine::enqueueRandom(std::string const & prefix) {
		std::vector<std::string> installed = bm_.getInstalledBehaviors();
		std::vector<std::string> matching;
		for (auto const & behavior : installed) {
			if (boost::starts_with(behavior, prefix)) matching.push_back(behavior);
		}
		
		boost::random::uniform_int_distribution<> range(0, matching.size() - 1);
		enqueue(matching[range(random_)]);
	}
	
	/// Join any background threads created by the engine.
	void BehaviorEngine::join() {
		if (wait_thread_.joinable()) wait_thread_.join();
	}
	
	/// Drop all queued jobs.
	/**
	 * If there is a job currently running, it will continue as normal.
	 */
	void BehaviorEngine::drop() {
		if (queue_.size() > 1) queue_.erase(queue_.begin() + 1, queue_.end());
	}
	
	/// Process the head of the queue.
	/**
	 * The job remains at the head of the queue until it finished.
	 */
	void BehaviorEngine::unqueue_() {
		BehaviorJob & job = queue_.front();
		
		// Call the start handler.
		if (job.on_start_) job.on_start_();
		
		RobotMessage message;
		message.mutable_behaviorcmd()->set_behaviorname(job.name_);
		engine->server.sendMessage(message);
		
		
		// If the behavior can be found, execute it.
		//std::cerr << "Trying to execute " << job.name_ << "." << std::endl;
		//if (bm_.isBehaviorInstalled(job.name_)) {
		//	job.id_ = bm_.post.runBehavior(job.name_);
			
			// Start a new wait thread.
			join();
			wait_thread_ = std::thread(std::bind(&BehaviorEngine::wait_, this));
		
		// Otherwise, immediately call the done handler.
		//} else {
			//if (job.on_done_) job.on_done_();
			//std::cout << "Immediately call the done handler because behavior was not found." << std::endl;
		//	onJobDone_();
	}
	
	/// Called when a job finished.
	void BehaviorEngine::onJobDone_() {
		// Invoke the done handler and pop the job.
		BehaviorJob & job = queue_.front();
		if (job.on_done_) job.on_done_();
		queue_.pop_front();
		
		//std::cout << "Dequeing job " << job.name_ << ", size " << queue_.size() << std::endl;
		
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
		while (!engine->behavior_done) {
		}
		engine->behavior_done = false;
		ios_.post(std::bind(&BehaviorEngine::onJobDone_, this));
		
		// If timeout is reached, then stop the behavior and drop the rest of the queue.
		/*if (bm_.wait(queue_.front().id_, BEHAVIOR_TIMEOUT)) {
			std::cerr << "Behavior timeout reached! Dropping queue." << std::endl;
			bm_.stop(queue_.front().id_);
			std::cerr << "Clearing queue!" << std::endl;
			queue_.clear();
		}
		else {*/
		//}
	}
}

