#pragma once

#include <boost/asio.hpp>

#include <alcommon/almodule.h>
#include <alproxies/albehaviormanagerproxy.h>

namespace AL {
	class ALBroker;
}

namespace robotutor {
	
	/**
	 * Behaviour engine executes behaviours in commands.
	 * All behaviours should be interruptable.
	 */
	class BehaviorEngine {
		protected:
			/// Behavior manager to use.
			AL::ALBehaviorManagerProxy bhv_;
		
		public:
			/// Construct the behaviour engine.
			/**
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			BehaviorEngine(boost::shared_ptr<AL::ALBroker> broker, boost::asio::io_service & ios);
			
			/// Run a behaviour asynchronously.
			/**
			 * \param name The name of the behaviour.
			 */
			play(const std::string &name);
			
			/// Stop a running behaviour.
			/**
			 * \param name The name of the behaviour.
			 */
			stop(const std::string &name);
	};
	
}
