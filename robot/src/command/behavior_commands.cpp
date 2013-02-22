#include <ostream>

#include <boost/lexical_cast.hpp>

#include "behavior_commands.hpp"
#include "../engine/script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Construct the command.
		/**
		 * \param arguments The arguments for the command.
		 */
		Behavior::Behavior(ArgList && arguments) {
			if (arguments.size() != 1) {
				throw std::runtime_error("Wrong amount of arguments for behavior command. Got " + boost::lexical_cast<std::string>(arguments.size()) + ", expected 1.");
			}
			
			command::Text::SharedPtr text = std::dynamic_pointer_cast<command::Text const>(arguments[0]);
			behavior = text->original;
			
			if (!behavior.size()) {
				throw std::runtime_error("Empty behavior name not allowed for behavior command.");
			}
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Behavior::run(ScriptEngine & engine) const {
			engine.behavior.run(behavior);
			return true;
		}
	}
}
