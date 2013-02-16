#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>
#include <memory>
#include <functional>
#include <stdexcept>


namespace robotutor {
	
	class ScriptEngine;
	
	/// Namespace to contain all commands.
	namespace command {
		
		class Command;
		class CommandFactory;
		
		/// Shared pointer for executables.
		typedef std::shared_ptr<Command const> SharedPtr;
		
		/// Argument list, containing shared pointers to commands.
		typedef std::vector<SharedPtr> ArgList;
		
		/// Base class for executables.
		class Command : public std::enable_shared_from_this<Command> {
			public:
				/// Arguments for the command.
				ArgList arguments;
				
				/// Construct a command without arguments.
				Command() {}
				
				/// Construct a command with arguments.
				Command(ArgList const & arguments) : arguments(arguments) {}
				
				/// Construct a command with arguments.
				Command(ArgList && arguments) : arguments(std::move(arguments)) {}
				
				/// Virtual destructor.
				virtual ~Command() {};
				
				/// Get the name of the command.
				/**
				 * \return The name of the command.
				 */
				virtual std::string name() const = 0;
				
				/// Run the command.
				/**
				 * \param engine The engine to use when run a command.
				 */
				virtual bool run(ScriptEngine & engine) const = 0;
				
				/// Write the command to a stream.
				/**
				 * \param stream The stream to write to.
				 */
				virtual void write(std::ostream & stream) const;
		};
		
		/// Command to execute other commands.
		struct Execute : public Command {
			
			/// Construct the command.
			Execute() : Command() {};
			
			/// Construct the command.
			Execute(ArgList && arguments) : Command(std::move(arguments)) {};
			
			/// Create the command.
			static SharedPtr create(std::string && name, ArgList && arguments) {
				return std::make_shared<Execute>(std::move(arguments));
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "execute"; };
			
			/// Execute one step.
			bool run(ScriptEngine & engine) const;
		};
		
		
		/// Write a command to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command);
		
		/// Create a command from a name and an argument list.
		class Factory {
			protected:
				/// Function type for creator functions.
				typedef std::function<SharedPtr (std::string &&, ArgList && arguments)> Creator;
				
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
				SharedPtr create(std::string && name, ArgList && args) {
					CreatorMap::iterator creator = creators_.find(name);
					if (creator == creators_.end()) throw std::runtime_error("Command `" + name + "' not found.");
					return creator->second(std::move(name), std::move(args));
				}
				
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
		};
		
	}
}
