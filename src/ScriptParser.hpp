#ifndef SCRIPT_PARSER_H_
#define SCRIPT_PARSER_H_

#include <iostream>
#include <string>
#include <vector>

namespace robotutor {

struct Command {
	std::string cmd;
	std::vector<std::string> args;
};

struct Script {
	std::string text;
	std::vector<Command> commands;
};

enum ParseState {
	PARSE_STATE_TEXT,
	PARSE_STATE_COMMAND,
	PARSE_STATE_ARGS,
};

class ScriptParser {
	protected:
		Script script;
		std::string text;
		std::string cmd;
		std::string arg;
		Command command;
		ParseState state;

	public:
		ScriptParser();
		~ScriptParser() {};

		template<typename InputIterator>
		void parse(InputIterator start, InputIterator end) {
			// Parse
			while (start != end) {
				consume(*start++);
			}

			script.text = text;
		}

	private:
		void consume(char c);

		Script getScript();
};

}

#endif // SCRIPT_PARSER_H_