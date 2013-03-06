#include <string>

#include "behavior_commands.hpp"
#include "../engine/script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Construct the command.
		/**
		 * \param arguments The arguments for the command.
		 */
		Behavior::Behavior(Command * parent, std::string behavior) :
			Command(parent),
			behavior(behavior) {}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Behavior::step(ScriptEngine & engine) {
			engine.behavior.run(behavior);
			engine.current = parent;
			return false;
		}
	}
}
