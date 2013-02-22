#pragma once

#include <alproxies/albehaviormanagerproxy.h>


namespace boost {
	namespace asio {
		class io_service;
	}
}

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
			/// IO service to perform asynchronous work.
			boost::asio::io_service & ios_;
			
			/// Behavior manager to use.
			AL::ALBehaviorManagerProxy bm_;
		
		public:
			/// Construct the behaviour engine.
			/**
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			BehaviorEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker);
			
			/// Run a behaviour asynchronously.
			/**
			 * \param name The name of the behaviour.
			 */
			void run(std::string const & name);
			
			/// Stop a running behaviour.
			/**
			 * \param name The name of the behaviour.
			 */
			void stop(std::string const & name);
	};
	
}
