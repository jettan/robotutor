#include "executable.hpp"
#include "engine/script_engine.hpp"

namespace robotutor {
	
	namespace executable {
		
		/// Write a text executable to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Text const & text) {
			stream << text.text;
			for (int i = 0; i < text.arguments.size(); ++i) {
				stream << "\n" << *text.arguments[i];
			}
			return stream;
		}
		
		/// Write a command to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Command const & command) {
			stream << "{" << command.name;
			for (int i = 0; i < command.arguments.size(); ++i) {
				stream << "|"  << *command.arguments[i];
			}
			return stream << "}";
		}
		
		/// Write any executable to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Executable const & executable) {
			if (executable::Text const * text = dynamic_cast<executable::Text const *>(&executable)) {
				return stream << *text;
			} else if (executable::Command const * command = dynamic_cast<executable::Command const *>(&executable)) {
				return stream << *command;
			}
			return stream;
		}
		
		bool Text::step(ScriptEngine & engine) {
			return true;
		}
	}
}
