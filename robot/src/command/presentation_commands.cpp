#include <ostream>

#include "presentation_commands.hpp"
#include "../engine/script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool NextSlide::run(ScriptEngine & engine) const {
			
			std::cout << "Next slide inputted!" << std::endl;
			engine.server.adjustSlide(+1);
			return true;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool PreviousSlide::run(ScriptEngine & engine) const {
			engine.server.adjustSlide(-1);
			return true;
		}
	}
}

