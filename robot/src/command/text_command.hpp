#pragma once

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
			typedef boost::shared_ptr<Text> SharedPtr;
			
			/// The text to synthesize.
			std::string text;
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string const name() const { return "Text"; }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool run(ScriptEngine & engine);
			
			/// Write the command to a stream.
			/**
			 * \param stream The stream to write to.
			 */
			void write(std::ostream & stream) const;
		};
		
	}
}
