#pragma once
#include <memory>

#include "command.hpp"

namespace robotutor {
	namespace command {
		/// Command to go to a different slide.
		struct Slide : public Command {
			
			/// The offset.
			int offset;
			
			/// If true, the offset is relative to the current slide.
			/// If false, the offset is relative to slide 0.
			bool relative;
			
			/// Construct a slide command.
			/**
			 * \param parent The parent command.
			 * \param offset The offset.
			 * \param If true, the offset is relative.
			 */
			Slide(Command * parent, int offset, bool relative);
			
			/// Create a slide command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &);
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "slide"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine);
		};
		
	}
}

