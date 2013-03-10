#include <stdexcept>

#include "command_factory.hpp"


namespace robotutor {
	namespace command {
		
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
		/// Load commands from a shared object.
		/**
		 * \param pathname The pathname of the shared object.
		 * \return True if the import succeeded.
		 */
		bool Factory::load(std::string const & pathname) {
			return false;
		}
		
		/// Load commands from a folder.
		/**
		 * Note that the return value indicated the amount of object loaded,
		 * not the total amount of commands loaded.
		 * 
		 * \param pathname The pathname of the folder containing the shared objects.
		 * \return The amount of objects loaded.
		 */
		unsigned int Factory::loadFolder(std::string const & pathname) {
			return 0;
		}
		
	}
}
