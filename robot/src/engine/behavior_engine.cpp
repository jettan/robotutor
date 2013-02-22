#include "behavior_engine.hpp"

namespace robotutor {

	/// Construct the behaviour engine.
	BehaviorEngine::BehaviorEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker) :
		ios_(ios),
		bm_(broker) {}
	
	void BehaviorEngine::run(std::string const & name) {
		if (bm_.isBehaviorInstalled(name)) {
			bm_.runBehavior(name);
		}
	}
}

