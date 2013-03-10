#include <stdexcept>

extern "C" {
#include <dlfcn.h>
}

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "command_factory.hpp"


namespace robotutor {
	namespace command {
		
		typedef void (* loadCommands) (Factory & factory);
		
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
			void * plugin = dlopen(pathname.c_str(), RTLD_LAZY);
			if (plugin) {
				loadCommands load = reinterpret_cast<loadCommands>(dlsym(plugin, "loadCommands"));
				if (load) {
					load(*this);
					return true;
				}
			}
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
			boost::filesystem::path path(pathname);
			unsigned int total = 0;
			if (boost::filesystem::is_directory(path)) {
				for (boost::filesystem::directory_iterator i(path); i != boost::filesystem::directory_iterator(); ++i) {
					if (i->path().extension() == ".so" && load(i->path().native())) ++total;
				}
			}
			return total;
		}
		
	}
}
