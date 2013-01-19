#include "script_parser.hpp"

namespace robotutor {
	
	using namespace parser;
	
	TextParser::TextParser() {
		state_ = STATE_TEXT;
		text.text.reserve(1024);
	}
	
	bool TextParser::consume(char c) {
		switch (state) {
			// Not parsing anything yet.
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
				
			// Parsing the command name.
			case STATE_COMMAND:
				if (arg_parser_.consume(c)) {
					state_ = STATE_TEXT;
					text.arguments.push_back(arg_parser_.command);
				}
				return false;
		}
	}

}
