#include "script_parser.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a text parser.
	TextParser::TextParser() {
		state_ = STATE_TEXT;
		text.text.reserve(1024);
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool TextParser::consume(char c) {
		switch (state) {
			// Parsing normal text.
			case STATE_TEXT:
				// An opening curly bracket starts a command.
				if (c == '{') {
					state_ = STATE_COMMAND;
					arg_parser_ = CommandParser();
					arg_parser_.consume(c);
					return false;
					
				// The rest is text.
				} else {
					text.text.push_back(c);
					return false;
				}
				
			// Parsing a command.
			case STATE_COMMAND:
				if (arg_parser_.consume(c)) {
					state_ = STATE_TEXT;
					text.arguments.push_back(arg_parser_.command);
				}
				return false;
		}
	}
	
	/// Inform the parser that there is no more input.
	/**
	 * \return True if the parser is in a valid stop state.
	 */
	bool TextParser::finish() {
		// Strip trailing and leading spaces from the string.
		trim(text.text);
		
		// Not finished we're still parsing a command.
		return state_ == STATE_TEXT;
	}

}
