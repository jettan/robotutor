#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>


namespace robotutor {
	
	namespace command {
		
		class Command;
		
		
		/// Create a command from a name and an argument list.
		class Factory {
			protected:
				/// Function type for creator functions.
				typedef std::function<std::shared_ptr<Command> (Command * parent, std::string && name, std::vector<std::string> && arguments, Factory & factory)> Creator;
				
				/// Map type for the creator map.
				typedef std::map<std::string, Creator> CreatorMap;
				
				/// Map holding creator functions.
				CreatorMap creators_;
				
			public:
				
				/// Create a command.
				/**
				 * \param name The name of the command.
				 * \param args The argument list for the command.
				 */
				std::shared_ptr<Command> create(Command * parent,  std::string && name, std::vector<std::string> && args);
				
				/// Register a creator.
				/**
				 * \param name The name of the command.
				 * \param creator The creator function to instantiate the command.
				 */
				void add(std::string const & name, Creator creator) {
					creators_[name] = creator;
				}
				
				/// Register a command.
				/**
				 * The type parameter must have a static create function of the right type.
				 *
				 * \param name The name of the command.
				 */
				template<typename T>
				void add(std::string const & name) {
					creators_[name] = &T::create;
				}
				
				/// Register a command.
				/**
				 * The type parameter must have a static create function
				 * and a static static_name method.
				 */
				template<typename T>
				void add() {
					creators_[T::static_name()] = &T::create;
				}
				
				/// Load commands from a shared object.
				/**
				 * \param pathname The pathname of the shared object.
				 * \return True if the import succeeded.
				 */
				bool load(std::string const & pathname);
				
				/// Load commands from a folder.
				/**
				 * Note that the return value indicated the amount of object loaded,
				 * not the total amount of commands loaded.
				 * 
				 * \param pathname The pathname of the folder containing the shared objects.
				 * \return The amount of objects loaded.
				 */
				unsigned int loadFolder(std::string const & pathname);
		};
	}
}
