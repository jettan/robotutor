#pragma once
#include <string>

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
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &);
			
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
