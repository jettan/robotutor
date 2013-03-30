#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>


namespace robotutor {
	
	class Plugin;
	
	namespace command {
		
		class Command;
		
		/// Create a command from a name and an argument list.
		class Factory {
			protected:
				/// Function type for creator functions.
				typedef std::function<std::shared_ptr<Command> (Command * parent, Plugin * plugin, std::vector<std::string> && arguments, Factory & factory)> Creator;
				
				struct Entry {
					Creator creator;
					Plugin * plugin;
				};
				
				/// Map type for the creator map.
				typedef std::map<std::string, Entry> CreatorMap;
				
				/// Map holding creator functions.
				CreatorMap creators_;
				
			public:
				/// Construct a command factory.
				/**
				 * The command factory will automatically load all core commands.
				 */
				Factory();
				
				/// Create a command.
				/**
				 * \param parent The parent command.
				 * \param args The argument list for the command.
				 */
				std::shared_ptr<Command> create(Command * parent, std::string const & name, std::vector<std::string> && args);
				
				/// Register a creator.
				/**
				 * \param name The name of the command.
				 * \param creator The creator function to instantiate the command.
				 * \param plugin The plugin of the command.
				 */
				void add(std::string const & name, Creator creator, Plugin * plugin = nullptr) {
					creators_[name] = {creator, plugin};
				}
				
				/// Register a command.
				/**
				 * \param plugin The plugin of the command.
				 */
				template<typename T>
				void add(Plugin * plugin = nullptr) {
					add(T::static_name(), &T::create, plugin);
				}
		};
	}
}
