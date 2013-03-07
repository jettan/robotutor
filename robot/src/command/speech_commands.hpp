#pragma once
#include <string>
#include <deque>
#include <memory>

#include "command.hpp"

namespace robotutor {
	namespace command {
		
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
			Speech(Command * parent, std::string const & text = "") :
				Command(parent),
				text(text),
				mark(0),
				synthesized(false) {}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "speech"; }
			
			/// Called when a bookmark is encountered.
			void onBookmark(ScriptEngine & engine, unsigned int bookmark);
			
			/// Called when the speech engine finished saying us.
			void onDone(ScriptEngine & engine, bool interrupted);
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 * \return True if an asynchronous operation was started.
			 */
			bool step(ScriptEngine & engine);
			
			/// Write the command to a stream.
			/**
			 * \param stream The stream to write to.
			 */
			void write(std::ostream & stream) const;
		};
		
		/// Command to stop the program execution.
		struct Stop : public Command {
			
			/// Construct a stop command.
			Stop(Command * parent) : Command(parent) {}
			
			/// Create a stop command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size()) throw std::runtime_error("Stop command takes zero arguments.");
				return std::make_shared<Stop>(parent);
			}
			
			/// The name of the command.
			static std::string static_name() { return "stop"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return Stop::static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine);
		};
	}
}
