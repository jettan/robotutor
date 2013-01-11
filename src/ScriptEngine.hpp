#include <iostream>
#include "ScriptParser.hpp"

namespace robotutor {

class ScriptEngine {

		void load(Script script);
		void step();
		void start();
		void stop();
		void reset();

        /*public:
		bookmarkHandler();
		onSentenceHook();
		onwordHook();
		onstartHook();
		onstopHook();
		onresetHook();
		onBookmark(); */
};

}
