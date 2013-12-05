#pragma once
#include <atomic>
#include <thread>
#include <functional>

#include <boost/random/mersenne_twister.hpp>

#include "command.hpp"
#include "command_factory.hpp"
#include "speech_engine.hpp"
#include "behavior_engine.hpp"
#include "robotutor_protocol.hpp"

namespace AL {
	class ALBroker;
}

namespace robotutor {
	
	/// Collection of engines required by commands.
	/**
	 * Commands get access to the script engine during executing.
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
			
			/// Command factory to use when parsing scripts.
			command::Factory factory;
			
			/// Random number generator.
			boost::random::mt19937 random;
			
			bool behavior_done;
		protected:
			/// The IO service to use.
			boost::asio::io_service & ios_;
			
			/// Registered plugins.
			std::vector<std::shared_ptr<Plugin>> plugins_;
			
			/// The root command.
			std::shared_ptr<command::Command> root_ { nullptr };
			
			/// The current command.
			command::Command * current_ { nullptr };
			
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
			
			/// Get the current command.
			/**
			 * \return The command currently executing.
			 */
			command::Command * current() { return current_; }
			
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
			
			/// Load a plugin from a shared library.
			/**
			 * \param name The name of the shared library.
			 */
			bool loadPlugin(std::string const & name);
			
			/// Load plugins from a directory.
			/**
			 * All files with a ".so" extension will be loaded as plugins.
			 *
			 * \param directory The directory containing the plugins.
			 */
			unsigned int loadPlugins(std::string const & directory);
			
		protected:
			/// Handle messages by passing them to all registered message handlers.
			/**
			 * \param connection The connection that sent the message.
			 * \param message The message.
			 */
			void handleMessage_(SharedServerConnection connection, ClientMessage && message);
			
			/// Wait for the engine to cleanly stop.
			/**
			 * \param handler The callback to invoke when the waiting is done.
			 */
			void wait_(std::function<void ()> callback = nullptr);
			
			/// Continue the script.
			void continue_();
	};
	
}
