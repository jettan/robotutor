#include <stdexcept>
#include <memory>

#include "behavior_commands.hpp"
#include "../script_engine.hpp"
#include "../command_factory.hpp"

namespace robotutor {
	namespace command {
		
		/// Load the commands in a factory.
		/**
		 * Load all commands of a plugin into a factory.
		 * 
		 * \param factory The factory to add the commands to.
		 */
		extern "C" void loadCommands(Factory & factory) {
			factory.add<Behavior>();
		}
		
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
			engine.behavior.enqueue(behavior);
			return done_(engine);
		}
	}
}
