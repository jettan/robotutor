#pragma once
#include <string>
#include <vector>
#include <memory>

#include "command.hpp"

namespace robotutor {
	namespace command {
		
		/// Text command.
		/**
		 * The normal text is synthesized by a TTS engine,
		 * the embedded commands are executed when a bookmark is encountered.
		 */
		struct Text : public Command {
			/// Shared pointer type.
			typedef std::shared_ptr<Text const> SharedPtr;
			
			/// The text to synthesize.
			std::vector<std::string> sentences;
			
			/// The last mark, per sentence.
			std::vector<int> marks;
			
			/// The original text.
			std::string original;
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "text"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool run(ScriptEngine & engine) const;
			
			/// Write the command to a stream.
			/**
			 * \param stream The stream to write to.
			 */
			void write(std::ostream & stream) const;
		};
		
		/// Command to stop the program execution.
		struct Stop : public Command {
			
			/// Create a stop command.
			static SharedPtr create(std::string && name, ArgList && arguments) {
				return std::make_shared<Stop>();
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return "stop"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool run(ScriptEngine & engine) const;
		};
	}
}
