#pragma once
#include <memory>

#include "command.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to go to the next slide.
		struct NextSlide : public Command {
			
			/// Create a next command.
			static SharedPtr create(std::string && name, ArgList && arguments) {
				return std::make_shared<NextSlide>();
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "next slide"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool run(ScriptEngine & engine) const;
		};
		
		/// Command to go to the previous slide.
		struct PreviousSlide : public Command {
			
			/// Create a next command.
			static SharedPtr create(std::string && name, ArgList && arguments) {
				return std::make_shared<PreviousSlide>();
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "previous slide"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool run(ScriptEngine & engine) const;
		};
	}
}

