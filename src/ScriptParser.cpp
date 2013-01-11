#include "ScriptParser.hpp"

namespace robotutor {

/**
 * Constructor for the ScriptParser.
 */
ScriptParser::ScriptParser() : state(PARSE_STATE_SENTENCE) {
}

/**
 * Consumes a char from the script, adding it to either a sentence or a command based on the current state.
 * \param c is the char that is consumed.
 */
void ScriptParser::consume(char c) {
	
	switch (c) {
		case '.': // end of sentence, add to sentences and clear the sentence string
			sentence += c;
			script.sentences.push_back(sentence);
			//std::cout << sentence << std::endl;
			sentence = "";
			break;;
		case '{': // starting a new command, change state
			state = PARSE_STATE_COMMAND;
			break;
		case ':': // parsing arguments for the command, change state
			state = PARSE_STATE_ARGS;
			break;
		case ',': // argument separator
			command.args.push_back(arg);
			//std::cout << "arg:" << arg << std::endl;
			arg = "";
			break;
		case '}': // end of command
			// we were parsing arguments so add the last one
			if (state == PARSE_STATE_ARGS) {
				command.args.push_back(arg);
				//std::cout << "arg:" << arg << std::endl;
			}

			state = PARSE_STATE_SENTENCE;

			//std::cout << "command:" << cmd << std::endl;
			command.cmd = cmd;
			cmd = "";
			arg = "";

			script.commands.push_back(command);
			break;
		default: // add char to string according to state
			switch (state) {
				case PARSE_STATE_SENTENCE:	sentence += c; break;
				case PARSE_STATE_COMMAND:	cmd += c; break;
				case PARSE_STATE_ARGS:		arg += c; break;
			}
			break;
	}
}

/**
 * Returns the parsed script.
 */
Script ScriptParser::getScript() {
	return script;
}

}

int main (int argc, char* argv[]) {
	if (argc != 2)
	{
		std::cout << "Please provide an argument string." << std::endl;
		return 0;
	}

	std::string text = argv[1];

	robotutor::ScriptParser sp;
	sp.parse(text.begin(), text.end());
	return 0;
}
