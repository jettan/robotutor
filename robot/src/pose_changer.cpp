#include <functional>
#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/chrono.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "pose_changer.hpp"
#include "behavior_engine.hpp"

namespace robotutor {
	
	/// Create a pose changer.
	/**
	 * \param ios The IO service to use.
	 * \param random The random generator to use.
	 * \param behavior_engine The behavior engine to control.
	 * \param prefix The prefix to select behaviors from.
	 * \param min The minimum time in milliseconds between two random poses.
	 * \param max The maximum time in milliseconds between two random poses.
	 */
	PoseChanger::PoseChanger(boost::asio::io_service & ios, boost::random::mt19937 & random, BehaviorEngine & behavior_engine, std::string prefix, unsigned int min, unsigned int max) :
		ios_(ios),
		random_(random),
		behavior_engine_(behavior_engine),
		timer_(ios),
		prefix(prefix),
		min(min),
		max(max)
	{
		started_.clear();
	}
	
	/// Start executing random behaviors.
	void PoseChanger::start() {
		asyncWaitRandom_();
	}
	
	/// Stop executing random behaviors.
	void PoseChanger::cancel() {
		timer_.cancel();
		started_.clear();
	}
	
	/// Wait a random period before calling the timeout function.
	void PoseChanger::asyncWaitRandom_() {
		if (!started_.test_and_set()) {
			boost::random::uniform_int_distribution<unsigned int> distribution(min, max);
			unsigned int timeout = distribution(random_);
			timer_.expires_from_now(std::chrono::milliseconds(timeout));
			timer_.async_wait(std::bind(&PoseChanger::handleTimeout_, this, std::placeholders::_1));
		}
	}
	
	
	/// Handle a timeout.
	/**
	 * \param error The error that occured, if any.
	 */
	void PoseChanger::handleTimeout_(boost::system::error_code const & error) {
		if (!error) {
			if (!behavior_engine_.queued()) behavior_engine_.enqueueRandom(prefix);
			asyncWaitRandom_();
		} else {
			started_.clear();
		}
	}
	
}
