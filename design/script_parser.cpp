class Script {
	Vector<String> sentences;
	Vector<Command> commands;
}

class ScriptParser {
	Script script;

	void consume(char c);
	void parse(InputIterator start, InputIterator end);
	Script getScript();
}

class ScriptEngine {

	load(Script script);
	step();
	start();
	stop();
	reset();

	bookmarkHandler_()

	hooks:
		onSentenceHook
		onwordHook
		onstartHook
		onstopHook
		onresetHook
		onBookmark
}

class Robotutor {
	ScriptEngine engine;
	
	void start();
	void stop();

	void pause();
	void resume();
}
