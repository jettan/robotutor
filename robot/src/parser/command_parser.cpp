#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include "command_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a text parser.
	CommandParser::CommandParser(command::Factory & factory) : factory_(factory) {
		text_ = boost::make_shared<command::Text>();
		state_  = STATE_TEXT;
		text_->text.reserve(1024);
		command_name_.reserve(32);
		command_args_.reserve(16);
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool CommandParser::consume(char c) {
		switch (state_) {
			// Parsing normal text.
			case STATE_TEXT:
				// An opening curly bracket starts a command.
				if (c == '{') {
					state_ = STATE_COMMAND_NAME;
					return false;
					
				// A colon or closing curly bracket ends the text.
				} else if (c == '|' || c == '}') {
					state_ = STATE_DONE;
					return true;
					
				// The rest is text.
				} else {
					text_->text.push_back(c);
					return false;
				}
				
			// Parsing a command name.
			case STATE_COMMAND_NAME:
				// Pipe symbol starts the argument list.
				if (c == '|') {
					state_ = STATE_COMMAND_ARGS;
					arg_parser_.reset(new CommandParser(factory_));
					return false;
					
				// A closing curly bracket closes the command.
				} else if (c == '}') {
					flushCommand_();
					state_ = STATE_TEXT;
					return false;
					
				// Lower case alpha characters, digits and normal spaces can be part of the name.
				} else if (isLowerAlpha(c) || isDigit(c) || c == ' ') {
					command_name_.push_back(c);
					return false;
					
				// Anything else is bogus.
				} else {
					throw std::runtime_error("Illegal character encountered in command name.");
				}
				
			// Parsing command arguments in sub-parser.
			case STATE_COMMAND_ARGS:
				if (arg_parser_->consume(c)) {
					arg_parser_->finish();
					command_args_.push_back(arg_parser_->result());
					if (c == '}') {
						flushCommand_();
						state_ = STATE_TEXT;
						return false;
					} else if (c == '|') {
						arg_parser_.reset(new CommandParser(factory_));
						return false;
					} else {
						throw std::runtime_error("Unexpected character finished command argument.");
					}
				} else {
					return false;
				}
				
			// Done parsing, stop feeding us.
			case STATE_DONE:
				throw std::runtime_error("Input received after command parsing finished.");
		}
	}
	
	/// Flush the recently parsed command.
	void CommandParser::flushCommand_() {
		trim(command_name_);
		text_->arguments.push_back(factory_.create(command_name_, command_args_));
		text_->text += "\\mrk=" + boost::lexical_cast<std::string>(text_->arguments.size()) + "\\";
		command_name_.clear();
		command_args_.clear();
	}
	
	/// Inform the parser that there is no more input.
	/**
	 * \return True if the parser is in a valid stop state.
	 */
	bool CommandParser::finish() {
		// Strip trailing and leading spaces from the string.
		trim(text_->text);
		
		// If we read exactly one command, just return that instead.
		if (text_->text == "\\mrk=1\\") {
			result_ = text_->arguments[0];
		} else {
			result_ = text_;
		}
		
		// Not finished if we're still parsing a command.
		return state_ == STATE_TEXT || state_ == STATE_DONE;
	}

}
