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
	
	class ScriptEngine;
	
	/**
	 * Behaviour engine executes behaviours in commands.
	 * All behaviours should be interruptable.
	 */
	class BehaviorEngine {
		protected:
			/// Reference to the parent script engine.
			ScriptEngine & parent_;
			
			/// IO service to perform asynchronous work.
			boost::asio::io_service & ios_;
			
			/// Behavior manager to use.
			AL::ALBehaviorManagerProxy bm_;
		
		public:
			/// Construct the behaviour engine.
			/**
			 * \param parent The parent script engine.
			 * \param ios The IO service to use.
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			BehaviorEngine(ScriptEngine & engine, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker);
			
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
			
			/// Returns a random number uniformly from given interval.
			/**
			 * \param lower
			 * \param upper
			 */
			 int rnd(int lower, int upper);
			 
			 /// Get processor clock value via assembly instruction
			 int rdtsc();
	};
	
}
