#include "behavior_engine.hpp"

namespace robotutor {

	/// Construct the behaviour engine.
	BehaviorEngine::BehaviorEngine(boost::shared_ptr<AL::ALBroker> broker, boost::asio::io_service & ios) : bhv_(broker) {}
	
}

