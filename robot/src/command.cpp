#include "command.hpp"
#include "script_engine.hpp"


namespace robotutor {
	
	namespace command {
		
		/// Write a command to a stream.
		/**
		 * \param stream  The stream to write to.
		 * \param command The command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command) {
			command.write(stream);
			return stream;
		}
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Command::write(std::ostream & stream) const {
			stream << "{" << name();
			for (auto argument : arguments) stream << "|"  << *argument;
			stream << "}";
		}
		
		/// Should be called when the command is done.
		/**
		 * Sets the current command of the engine to the parent of this command.
		 * 
		 * \param engine The script engine to modify.
		 * \return True.
		 */
		bool Command::done_(ScriptEngine & engine) const {
			engine.current = parent;
			return true;
		}
		
	}
}
