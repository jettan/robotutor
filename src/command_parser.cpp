#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	CommandParser::CommandParser() {
		state_ = STATE_START;
		name_.reserve(30);
	}
	
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
					arg_parser_ = ExecutableParser();
					return false;
					
				// A closing curly bracket closes the command.
				} else if (c == '}') {
					flush_();
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
					return false;
					
				// Closing bracket ends the command.
				} else if (c == '}') {
					flush_arg_();
					flush_();
					return true;
					
				// The rest is fed to the argument parser.
				} else {
					arg_parser_.consume(c);
					return false;
				}
		}
	}
	
	void CommandParser::flush_arg_() {
		args_.push_back(arg_parser_.executable);
		arg_parser_ = ExecutableParser();
	}
	
	void CommandParser::flush_() {
		command = executable::CommandFactory::create(name_, args_);
	}

}
