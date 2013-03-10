#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <memory>


namespace robotutor {
	
	class ScriptEngine;
	
	/// Namespace to contain all commands.
	namespace command {
		
		class Command;
		class Factory;
		
		/// Shared pointer for executables.
		typedef std::shared_ptr<Command> SharedPtr;
		
		/// Argument list, containing shared pointers to commands.
		typedef std::vector<SharedPtr> ArgList;
		
		/// Base class for executables.
		class Command : public std::enable_shared_from_this<Command> {
			public:
				/// The parent command.
				Command * parent;
				
				/// Arguments for the command.
				ArgList arguments;
				
				/// Construct a command without arguments.
				Command(Command * parent = nullptr) : parent(parent) {}
				
				/// Construct a command with arguments.
				Command(Command * parent, ArgList const & arguments) : arguments(arguments) {}
				
				/// Construct a command with arguments.
				Command(Command * parent, ArgList && arguments) : arguments(std::move(arguments)) {}
				
				/// Virtual destructor.
				virtual ~Command() {};
				
				/// Run the command.
				/**
				 * \param engine The engine to use when run a command.
				 * \return True if the engine should continue processing commands.
				 */
				virtual bool step(ScriptEngine & engine) = 0;
				
				/// Write the command to a stream.
				/**
				 * \param stream The stream to write to.
				 */
				virtual void write(std::ostream & stream) const;
				
				/// Get the name of the command.
				/**
				 * \return The name of the command.
				 */
				virtual std::string name() const = 0;
				
			protected:
				/// Should be called when the command is done.
				/**
				 * Sets the current command of the engine to the parent of this command.
				 * 
				 * \param engine The script engine to modify.
				 * \return True.
				 */
				bool done_(ScriptEngine & engine) const;
				
		};
		
		/// Write a command to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command);
		
	}
}
