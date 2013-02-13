#include <istream>
#include <streambuf>
#include <string>
#include <vector>
#include <stdexcept>
#pragma once

#include <iterator>
#include <memory>

#include "../command/command.hpp"
#include "../command/text_command.hpp"

namespace robotutor {
	
	/// Parser for text executables.
	/**
	 * This parser will read an entire text executable from the input.
	 */
	class CommandParser {
		protected:
			/// The state of the parser.
			enum {
				STATE_TEXT,
				STATE_COMMAND_NAME,
				STATE_COMMAND_ARGS,
				STATE_DONE
			} state_;
			
			/// Factory to create commands.
			command::Factory & factory_;
			
			/// Name of the command currently being parsed.
			std::string command_name_;
			
			/// Arguments for the command currently being parsed.
			command::ArgList command_args_;
			
			/// Parser for embedded commands.
			std::unique_ptr<CommandParser> arg_parser_;
			
			/// The text executable used as buffer.
			command::Text::SharedPtr text_;
			
			/// The parse result.
			command::SharedPtr result_;
			
		public:
			/// Construct a text parser.
			/**
			 * \param factory The command factory to use to instantiate commands found in the text.
			 */
			CommandParser(command::Factory & factory);
			
			/// Get the parse result.
			/**
			 * May only be called after a call to finish().
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			command::SharedPtr result() const { return result_; }
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
			/// Inform the parser that there is no more input.
			/**
			 * \return True if the parser is in a valid stop state.
			 */
			bool finish();
			
		protected:
			/// Flush the recently parsed command.
			void flushCommand_();
	};
	
	
}
