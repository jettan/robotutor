#include <stdexcept>

#include "command_factory.hpp"
#include "core_commands.hpp"


namespace robotutor {
	namespace command {
		
		/// Construct a command factory.
		/**
		 * The command factory will automatically load all core commands.
		 */
		Factory::Factory() {
			add<command::Execute>();
			add<command::Stop>();
		}
		
		/// Create a command.
		/**
		 * \param name The name of the command.
		 * \param args The argument list for the command.
		 */
		std::shared_ptr<Command> Factory::create(Command * parent,  std::string && name, std::vector<std::string> && args) {
			CreatorMap::iterator creator = creators_.find(name);
			if (creator == creators_.end()) throw std::runtime_error("Command `" + name + "' not found.");
			return creator->second(parent, std::move(name), std::move(args), *this);
		}
		
	}
}
