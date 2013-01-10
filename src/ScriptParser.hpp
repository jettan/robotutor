#include <iostream>
#include <string>
#include <vector>

namespace robotutor {

struct Command {
	std::string cmd;
	std::vector<std::string> args;
};

struct Script {
	std::vector<std::string> sentences;
	std::vector<Command> commands;
};

class ScriptParser {
	protected:
		Script script;
		std::string sentence;
		int bookmark;
	public:
		ScriptParser();
		~ScriptParser();

	private:
		void consume(char c);

		template<typename InputIterator>
		void parse(InputIterator start, InputIterator end) {
			// Parse
			while (start != end) {
				consume(*start++);
			}
		}

		Script getScript();
};

}
