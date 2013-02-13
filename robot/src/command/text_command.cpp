#include <ostream>

#include "text_command.hpp"

namespace robotutor {
	namespace command {
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Text::write(std::ostream & stream) const {
			stream << text;
			for (auto argument : arguments) stream << "\n" << *argument;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Text::run(ScriptEngine & engine) {
			return true;
		}
	}
}
