#include <stdexcept>
#include <memory>

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
		
		/// Create the command.
		SharedPtr Behavior::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
			if (arguments.size() != 1) throw std::runtime_error("Behavior command expects 1 argument.");
			return std::make_shared<Behavior>(parent, arguments[0]);
		}
		
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
