#include "command.hpp"

namespace robotutor {
	
	namespace command {
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 * */
		void Command::write(std::ostream & stream) const {
			stream << "{" << name();
			for (auto argument : arguments) stream << "|"  << *argument;
			stream << "}";
		}
		
		/// Write a command to a stream.
		/**
		 * \param stream  The stream to write to.
		 * \param command The command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command) {
			command.write(stream);
			return stream;
		}
		
		/// Run the command.
		/**
		 * \param engine The engine to use when run a command.
		 */
		bool Execute::run(ScriptEngine & engine) const {
			for (auto arg : arguments) {
				arg->run(engine);
			}
			return false;
		}
		
	}
}
