#pragma once
#include "../command/command.hpp"
#include "../server/server.hpp"
#include "speech_engine.hpp"

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
			/// The text-to-speech engine.
			boost::shared_ptr<SpeechEngine> speech;
			Server server;
			
			
			/// Construct the script engine.
			/**
			 * \param broker The ALBroker to use for communicating with naoqi.
			 */
			ScriptEngine(boost::shared_ptr<AL::ALBroker> broker, boost::asio::io_service & ios);
			
			/// Run a command with the script engine.
			/**
			 * \param command The command to run.
			 */
			void run(command::SharedPtr command) {
				command->run(*this);
			}
	};
	
}
