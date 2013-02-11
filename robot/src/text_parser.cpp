#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a text parser.
	TextParser::TextParser(executable::CommandFactory & factory) : factory_(factory) {
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
	executable::Text::SharedPtr TextParser::result() {
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
					arg_parser_.reset(new CommandParser(factory_));
					arg_parser_->consume(c);
					return false;
					
				// A colon or closing curly bracket ends the text.
				} else if (c == '|' || c == '}') {
					state_ = STATE_DONE;
					return true;
					
				// The rest is text.
				} else {
					result_->text.push_back(c);
					return false;
				}
				
			// Parsing a command.
			case STATE_COMMAND:
				if (arg_parser_->consume(c)) {
					std::cout << "Finished reading command." << std::endl;
					state_ = STATE_TEXT;
					arg_parser_->finish();
					result_->arguments.push_back(arg_parser_->result());
					result_->text += "\\mrk=" + boost::lexical_cast<std::string>(result_->arguments.size()) + "\\";
				}
				return false;
				
			// Done parsing, stop feeding us.
			case STATE_DONE:
				throw std::runtime_error("Input received after command parsing finished.");
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
		return state_ == STATE_TEXT || state_ == STATE_DONE;
	}

}
