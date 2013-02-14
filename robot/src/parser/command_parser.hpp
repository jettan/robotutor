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
			
			/// Buffer for the text contents.
			std::string text_;
			
			/// Buffer for embedded commands.
			command::ArgList commands_;
			
			/// Name of the command currently being parsed.
			std::string command_name_;
			
			/// Arguments for the command currently being parsed.
			command::ArgList command_args_;
			
			/// Parser for embedded commands.
			std::unique_ptr<CommandParser> arg_parser_;
			
		public:
			/// Construct a text parser.
			/**
			 * \param factory The command factory to use to instantiate commands found in the text.
			 */
			CommandParser(command::Factory & factory);
			
			/// Reset the parser so that it can parse a new command.
			void reset();
			
			/// Get the parse result.
			/**
			 * May throw an exception if the parser is not in a valid state to return a result.
			 * If no exception is thrown, the parser is reset.
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			command::SharedPtr result();
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
		protected:
			/// Flush the recently parsed command.
			void flushCommand_();
	};
	
	
}
