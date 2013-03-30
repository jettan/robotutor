#include <iostream>
#include <fstream>
#include <stdexcept>

#include "../script_engine.hpp"
#include "../robotutor_protocol.hpp"
#include "../script_parser.hpp"

namespace robotutor {
	namespace {
		
		/// Handle script messages.
		void handleScriptMessage(ScriptEngine & engine, SharedServerConnection connection, ClientMessage const & message) {
			if (message.has_run()) {
				std::shared_ptr<command::Command> script;
				
				// Check if there is a script to parse.
				try {
					if (message.run().has_script()) {
						script = parseScript(engine.factory, message.run().script());
					} else if (message.run().has_file()) {
						std::ifstream stream(message.run().file());
						if (!stream.good()) throw std::runtime_error("Failed to open file `" + message.run().file() + "'.");
						script =  parseScript(engine.factory, stream);
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
						engine.ios().post([&engine, script] () {
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
		void handleControlMessage(ScriptEngine & engine, SharedServerConnection connection, ClientMessage const & message) {
			if (message.has_stop()) {
				engine.stop();
				engine.join();
				engine.load(nullptr);
			} else if (message.has_pause()) {
				engine.stop();
				engine.join();
			} else if (message.has_resume()) {
				engine.start();
			}
		}
		
	}
	
	/// Register the plugin with a script engine.
	/**
	 * \param engine The script engine.
	 */
	extern "C" bool registerPlugin(ScriptEngine & engine) {
		engine.addMessageHandler(handleScriptMessage);
		engine.addMessageHandler(handleControlMessage);
		return true;
	}
}


