#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	/// Construct a command parser.
	CommandParser::CommandParser(executable::CommandFactory & factory) : factory_(factory) {
		state_ = STATE_START;
		name_.reserve(30);
	}
	
	/// Get the parse result.
	/**
	 * May only be called after a call to finish().
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	executable::Command::SharedPtr CommandParser::result() {
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
				// Pipe symbol seperates arguments.
				if (c == '|') {
					flush_arg_();
					arg_parser_.reset(new ExecutableParser(factory_));
					return false;
					
				// Closing bracket ends the command.
				} else if (c == '}') {
					flush_arg_();
					state_ = STATE_DONE;
					return true;
					
				// The rest is fed to the argument parser.
				} else {
					arg_parser_->consume(c);
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
		if (state_ == STATE_DONE) {
			result_ = factory_.create(name_, args_);
			return true;
		} else {
			return false;
		}
	}
	
	/// Flush the argument buffer.
	void CommandParser::flush_arg_() {
		// Add the parsed argument to our list.
		arg_parser_->finish();
		args_.push_back(arg_parser_->result());
	}

}
