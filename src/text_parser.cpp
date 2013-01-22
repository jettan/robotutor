#include <boost/make_shared.hpp>

#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a text parser.
	TextParser::TextParser() {
		result_ = boost::make_shared<executable::Text>();
		state_  = STATE_TEXT;
		result_->text.reserve(1024);
	}
	
	/// Get the parse result.
	/**
	 * May only be called after a call to finish().
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	executable::SharedPtr ExecutableParser::result() {
		return result_;
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool TextParser::consume(char c) {
		switch (state_) {
			// Parsing normal text.
			case STATE_TEXT:
				// An opening curly bracket starts a command.
				if (c == '{') {
					state_ = STATE_COMMAND;
					arg_parser_.reset(new CommandParser());
					arg_parser_->consume(c);
					return false;
					
				// The rest is text.
				} else {
					result_->text.push_back(c);
					return false;
				}
				
			// Parsing a command.
			case STATE_COMMAND:
				if (arg_parser_->consume(c)) {
					state_ = STATE_TEXT;
					arg_parser_->finish();
					result_->arguments.push_back(arg_parser_->result());
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
		trim(result_->text);
		
		// Not finished if we're still parsing a command.
		return state_ == STATE_TEXT;
	}

}
