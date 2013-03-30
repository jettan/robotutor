#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <memory>


namespace robotutor {
	
	class ScriptEngine;
	class Plugin;
	
	/// Namespace to contain all commands.
	namespace command {
		
		class Command;
		class Factory;
		
		/// Shared pointer for executables.
		typedef std::shared_ptr<Command> SharedPtr;
		
		/// Argument list, containing shared pointers to commands.
		typedef std::vector<SharedPtr> CommandList;
		
		/// Base class for executables.
		class Command : public std::enable_shared_from_this<Command> {
			public:
				/// The parent command.
				Command * parent;
				
				/// The plugin for the command.
				Plugin * plugin;
				
				/// Arguments for the command.
				CommandList children;
				
				/// Construct a command without arguments.
				Command(Command * parent, Plugin * plugin) :
					parent(parent),
					plugin(plugin) {}
				
				/// Construct a command with children.
				template<typename Children>
				Command(Command * parent, Plugin * plugin, Children && children) :
					parent(parent),
					plugin(plugin),
					children(std::forward<Children>(children)) {}
				
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
				/// Set the next command to be executed.
				/**
				 * \param next The next command to execute.
				 */
				void setNext_(ScriptEngine & engine, Command * next);
				
				/// Continue the script engine.
				/**
				 * Should be called by commands when an asynchronous operation completed.
				 */
				void continue_(ScriptEngine & engine);
				
				/// Should be called when the command is done.
				/**
				 * Sets the current command of the engine to the parent of this command.
				 * 
				 * \param engine The script engine to modify.
				 * \return True.
				 */
				bool done_(ScriptEngine & engine);
				
		};
		
		/// Write a command to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command);
		
	}
}
