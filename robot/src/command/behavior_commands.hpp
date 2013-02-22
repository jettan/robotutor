#pragma once
#include <string>
#include <vector>
#include <memory>

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
			Behavior(ArgList && arguments);
			
			/// Create the command.
			static SharedPtr create(std::string && name, ArgList && arguments) {
				return std::make_shared<Behavior>(std::forward<ArgList>(arguments));
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
			bool run(ScriptEngine & engine) const;
		};
	}
}
