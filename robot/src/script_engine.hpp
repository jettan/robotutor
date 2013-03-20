#pragma once
#include <atomic>
#include <thread>

#include <boost/signal.hpp>

#include "command.hpp"
#include "speech_engine.hpp"
#include "behavior_engine.hpp"
#include "robotutor_protocol.hpp"

namespace AL {
	class ALBroker;
}

namespace robotutor {
	
	namespace command {
		class Command;
	}
	
	/// Collection of engines required by commands.
	/**
	 * Commands get access to the script engine during executing.
	 * All state must be saved in the script engine, commands may not keep state.
	 */
	class ScriptEngine {
		friend class command::Command;
		public:
			/// The AL broker for naoqi communication.
			boost::shared_ptr<AL::ALBroker> broker;
			
			/// The text-to-speech engine.
			boost::shared_ptr<SpeechEngine> speech;
			
			/// The behavior engine.
			BehaviorEngine behavior;
			
			/// The server engine.
			Server server;
			
			/// Signal invoked when the script finished execution.
			boost::signal<void ()> on_done;
			
		protected:
			/// The IO service to use.
			boost::asio::io_service & ios_;
			
			/// The root command.
			std::shared_ptr<command::Command> root_ = nullptr;
			
			/// The current command.
			command::Command * current_ = nullptr;
			
			/// True if the engine is started.
			std::atomic_bool started_ { false };
			
			/// Thread to wait in the background.
			std::thread wait_thread_;
			
		public:
			/// Construct the script engine.
			/**
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			ScriptEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker);
			
			/// Load a script.
			void load(std::shared_ptr<command::Command> script);
			
			/// Get the IO service used by the engine.
			/**
			 * \return The IO service.
			 */
			boost::asio::io_service & ios() { return ios_; }
			
			/// Check if the engine is executing a script.
			/**
			 * \return True if the engine is currently executing a script.
			 */
			bool started() { return started_; }
			
			/// Join any background threads created by the engine.
			/**
			 * Make sure that the IO service has already been stopped,
			 * or it may still process events that use the engine.
			 */
			void join();
			
			/// Start the engine.
			void start();
			
			/// Stop the engine as soon as possible.
			/**
			 * \param callback Callback to invoke when the engine was stopped.
			 */
			void stop(std::function<void ()> handler = nullptr);
			
		protected:
			/// Wait for the engine to cleanly stop.
			/**
			 * \param handler The callback to invoke when the waiting is done.
			 */
			void wait_(std::function<void ()> callback = nullptr);
			
			/// Continue the script.
			void continue_();
	};
	
}
