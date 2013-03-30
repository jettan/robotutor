extern "C" {
#include <dlfcn.h>
}

#include "plugin.hpp"

namespace robotutor {
	
	/// Descontruct a plugin.
	/**
	 * Closes the handle using dlclose().
	 */
	Plugin::~Plugin() {
		if (handle_) dlclose(handle_);
	}
	
	/// Create a plugin from a file.
	/**
	 * \param file The file containing the plugin.
	 * \param engine The script engine the plugin is for.
	 */
	std::shared_ptr<Plugin> Plugin::load(std::string const & file, ScriptEngine & engine) {
		void * handle = dlopen(file.c_str(), RTLD_LAZY | RTLD_GLOBAL);
		if (handle) {
			auto create_plugin = reinterpret_cast<createPlugin>(dlsym(handle, "createPlugin"));
			if (create_plugin) {
				std::shared_ptr<Plugin> plugin(create_plugin(engine));
				if (plugin) {
					plugin->handle_ = handle;
					return plugin;
				}
			}
			dlclose(handle);
		}
		return nullptr;
	}
}
