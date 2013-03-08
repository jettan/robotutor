#pragma once

#include <boost/signal.hpp>

#include "../command/command.hpp"
#include "speech_engine.hpp"
#include "behavior_engine.hpp"
#include "server_engine.hpp"

namespace AL {
	class ALBroker;
}

namespace robotutor {
	
	/// Collection of engines required by commands.
	/**
	 * Commands get access to the script engine during executing.
	 * All state must be saved in the script engine, commands may not keep state.
	 */
	class ScriptEngine {
		public:
			/// The AL broker for naoqi communication.
			boost::shared_ptr<AL::ALBroker> broker;
			
			/// The text-to-speech engine.
			boost::shared_ptr<SpeechEngine> speech;
			
			/// The behavior engine.
			BehaviorEngine behavior;
			
			/// The server engine.
			ServerEngine server;
			
			/// The current command.
			command::Command * current;
			
			/// Signal invoked when the script finished execution.
			boost::signal<void ()> on_done;
			
		protected:
			/// The root command.
			std::shared_ptr<command::Command> root_;
			
			/// Thread to wait in the background.
			std::thread wait_thread_;
			
		public:
			/// Construct the script engine.
			/**
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			ScriptEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker);
			
			/// Load a script.
			void load(std::shared_ptr<command::Command> script) {
				root_ = script;
				current = root_.get();
				std::cout << "Script loaded:\n" << *root_ << std::endl;
			}
			
			/// Check if the engine is executing a script.
			/**
			 * \return True if the engine is currently running a script.
			 */
			bool busy() { return current; }
			
			/// Join any background threads created by the engine.
			/**
			 * Make sure that the IO service has already been stopped,
			 * or it may still process events that use the engine.
			 */
			void join();
			
			/// Stop the engine as soon as possible.
			/**
			 * \param callback Callback to invoke when the engine was stopped.
			 */
			void stop(std::function<void ()> handler = nullptr);
			
			/// Run the script.
			void run();
			
		protected:
			/// Wait for the engine to cleanly stop.
			/**
			 * \param handler The callback to invoke when the waiting is done.
			 */
			void wait_(std::function<void ()> callback = nullptr);
	};
	
}
