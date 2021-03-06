#include <iostream>
#include <fstream>
#include <stdexcept>

#include "../plugin.hpp"
#include "../script_engine.hpp"
#include "../robotutor_protocol.hpp"
#include "../script_parser.hpp"

namespace robotutor {
	
	/// Plugin to react to control 
	struct ControlPlugin : public Plugin {
		ControlPlugin(ScriptEngine & engine) :
			Plugin(engine) {}
		
		/// Process server messages.
		/**
		 * \param connection The connection that sent the message.
		 * \param message The message.
		 */
		void handleMessage(SharedServerConnection connection, ClientMessage const & message) {
			handleScriptMessage(connection, message);
			handleControlMessage(connection, message);
		}
		
		/// Handle script messages.
		void handleScriptMessage(SharedServerConnection connection, ClientMessage const & message) {
			if (message.has_run()) {
				std::shared_ptr<command::Command> script;
				
				// Check if there is a script to parse.
				try {
					if (message.run().has_script()) {
						script = parseScript(engine, message.run().script());
					} else if (message.run().has_file()) {
						std::ifstream stream(message.run().file());
						if (!stream.good()) throw std::runtime_error("Failed to open file `" + message.run().file() + "'.");
						script =  parseScript(engine, stream);
					}
				} catch (std::exception const & e) {
					std::cout << "Error parsing script: " << e.what() << std::endl;
				}
				
				// Run the parsed script.
				if (script) {
					std::cout << "Script parsed." << std::endl;
					std::cout << *script << std::endl;
					// If the engine is busy, stop it and wait for everything to finish before running the new script.
					if (engine.started()) {
						engine.stop();
						engine.join();
						engine.ios().post([this, script] () {
							engine.load(script);
							engine.start();
						});
						
					// If the engine wasn't busy, just run the script.
					} else {
						engine.load(script);
						engine.start();
					}
				}
			}
		}
		
		/// Handle control messages.
		void handleControlMessage(SharedServerConnection connection, ClientMessage const & message) {
			if (message.has_stop()) {
				engine.stop();
				engine.join();
				engine.load(nullptr);
			} else if (message.has_pause()) {
				engine.stop();
				engine.join();
			} else if (message.has_resume()) {
				engine.start();
			} else if (message.has_behaviorcmd()) {
				std::cout << "We received from Junchao: " << message.behaviorcmd().succes() << std::endl;
				engine.behavior_done = true;
			}
		}
		
	};
	
	/// Register the plugin with a script engine.
	/**
	 * \param engine The script engine.
	 */
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new ControlPlugin(engine);
	}
}


