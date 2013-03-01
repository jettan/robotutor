#include "behavior_engine.hpp"
#include <stdint.h>

namespace robotutor {

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
	
	/// Run a behaviour asynchronously.
	/**
	 * \param name The name of the behaviour.
	 */
	void BehaviorEngine::run(std::string const & name) {
		if (bm_.isBehaviorInstalled(name)) {
			std::cout << "Running behaviour: `" << name << "'." << std::endl;
			bm_.post.runBehavior(name);
		} else {
			std::cout << "Behavior not found: `" << name << "'." << std::endl;
		}
	}
	
	int BehaviorEngine::rdtsc() {
		asm volatile("rdtsc");
	}
	
	///Draws a random number within the given bounds
	int BehaviorEngine::rnd(int lower, int upper) {
		srand(rdtsc());
		// Of: srand(time(NULL));
			int result = rand() % upper + lower;
			return result;
	}
}

