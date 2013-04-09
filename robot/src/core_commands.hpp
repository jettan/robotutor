#pragma once
#include <string>
#include <deque>
#include <memory>
#include <ostream>

#include "command.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to execute other commands.
		struct Execute : public Command {
			
			/// Next subcommand.
			unsigned int next;
			
			/// Construct the command.
			Execute(ScriptEngine & engine, Command * parent = nullptr) :
				Command(engine, parent, nullptr),
				next(0) {}
			
			/// Construct the command.
			template<typename Children>
			Execute(ScriptEngine & engine, Command * parent, Children && children) :
				Command(engine, parent, nullptr, std::forward<Children>(children)),
				next(0) {}
			
			/// Create the command.
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin *, std::vector<std::string> && arguments);
			
			/// The name of the command.
			static std::string static_name() { return "execute"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return Execute::static_name(); }
			
			/// Execute one step.
			bool step();
		};
		
		/// Text command.
		/**
		 * The normal text is synthesized by a TTS engine,
		 * the embedded commands are executed when a bookmark is encountered.
		 */
		struct Speech : public Command {
			/// The text to say.
			std::string text;
			
			/// Last executed bookmark.
			unsigned int mark;
			
			/// True if the text has been synthesized.
			bool synthesized;
			
			/// Queue for delayed commands.
			std::deque<Command *> delayed;
			
			/// Construct a sentence command.
			Speech(ScriptEngine & engine, Command * parent, std::string const & text = "") :
				Command(engine, parent, nullptr),
				text(text),
				mark(0),
				synthesized(false) {}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "speech"; }
			
			/// Called when a bookmark is encountered.
			void onBookmark(unsigned int bookmark);
			
			/// Called when the speech engine finished saying us.
			void onDone(bool interrupted);
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 * \return True if an asynchronous operation was started.
			 */
			bool step();
			
			/// Write the command to a stream.
			/**
			 * \param stream The stream to write to.
			 */
			void write(std::ostream & stream) const;
		};
		
		/// Command to stop the program execution.
		struct Stop : public Command {
			
			/// Construct a stop command.
			Stop(ScriptEngine & engine, Command * parent) :
				Command(engine, parent, nullptr) {}
			
			/// Create a stop command.
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin *, std::vector<std::string> && arguments);
			
			/// The name of the command.
			static std::string static_name() { return "stop"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step();
		};
	}
}
