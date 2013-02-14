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
		
		/// Abstract base class for all commands.
		struct Command;
		
		/// Shared pointer for executables.
		typedef std::shared_ptr<Command> SharedPtr;
		
		/// Argument list, containing shared pointers to commands.
		typedef std::vector<SharedPtr> ArgList;
		
		/// Base class for executables.
		struct Command {
			
			/// Arguments for the command.
			ArgList const arguments;
			
			/// Construct a command.
			Command(ArgList const & arguments) : arguments(arguments) {}
			
			/// Construct a command.
			Command(ArgList && arguments) : arguments(std::move(arguments)) {}
			
			/// Virtual destructor.
			virtual ~Command() {};
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			virtual std::string const name() const = 0;
			
			/// Execute one step.
			virtual bool run(ScriptEngine & engine) const = 0;
			
			/// Write the command to a stream.
			/**
			 * \param stream The stream to write to.
			 */
			virtual void write(std::ostream & stream) const;
			
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
		};
		
	}
}
