#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a command parser.
	CommandParser::CommandParser(command::Factory & factory) : factory_(factory) {
		state_ = STATE_START;
		name_.reserve(30);
	}
	
	/// Get the parse result.
	/**
	 * May only be called after a call to finish().
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	command::SharedPtr CommandParser::result() {
		return result_;
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool CommandParser::consume(char c) {
		switch (state_) {
			// Not parsing anything yet.
			case STATE_START:
				// Skip spaces.
				if (isSpace(c)) {
					return false;
				
				// First { opens the command.
				} else if (c == '{') {
					state_ = STATE_NAME;
					return false;
				
				// The rest is bogus.
				} else {
					throw std::runtime_error("Illegal character encountered while parsing command.");
				}
				
			// Parsing the command name.
			case STATE_NAME:
				// Pipe symbol starts the argument list.
				if (c == '|') {
					state_ = STATE_ARGS;
					arg_parser_.reset(new ExecutableParser(factory_));
					return false;
					
				// A closing curly bracket closes the command.
				} else if (c == '}') {
					state_ = STATE_DONE;
					return true;
					
				// Lower case alpha characters, digits and normal spaces can be part of the name.
				} else if (isLowerAlpha(c) || isDigit(c) || c == ' ') {
					name_.push_back(c);
					return false;
					
				// Anything else is bogus.
				} else {
					throw std::runtime_error("Illegal character encountered in command name.");
				}
				
			// Currently parsing arguments.
			case STATE_ARGS:
				if (arg_parser_->consume(c)) {
					arg_parser_->finish();
					args_.push_back(arg_parser_->result());
					if (c == '}') {
						state_ = STATE_DONE;
						return true;
					} else if (c == '|') {
						arg_parser_.reset(new ExecutableParser(factory_));
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
	
	/// Inform the parser that there is no more input.
	/**
	 * \return True if the parser is in a valid stop state.
	 */
	bool CommandParser::finish() {
		trim(name_);
		if (state_ == STATE_DONE) {
			result_ = factory_.create(name_, args_);
			return true;
		} else {
			return false;
		}
	}
	
}
