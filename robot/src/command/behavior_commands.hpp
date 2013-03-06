#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "command.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to run a behavior.
		struct Behavior : public Command {
			/// The behavior to play.
			std::string behavior;
			
			/// Construct the command.
			/**
			 * \param arguments The arguments for the command.
			 */
			Behavior(Command * parent, std::string behavior);
			
			/// Create the command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Behavior command expects 1 argument.");
				return std::make_shared<Behavior>(parent, arguments[0]);
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "behaviour"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine);
		};
	}
}
