#include <ostream>

#include "speech_commands.hpp"
#include "../engine/script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Text::write(std::ostream & stream) const {
			for (auto & sentence : sentences) stream << sentence << " ";
			for (auto & argument : arguments) stream << "\n" << *argument;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Text::run(ScriptEngine & engine) const {
			engine.speech->interrupt(std::dynamic_pointer_cast<Text const>(shared_from_this()));
			return true;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Stop::run(ScriptEngine & engine) const {
			engine.speech->pause();
			return true;
		}
	}
}
