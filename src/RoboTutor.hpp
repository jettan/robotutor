#include <iostream>
#include "ScriptEngine.hpp"

namespace robotutor {

class RoboTutor {

	protected:
		ScriptEngine engine;

	private:
		void start();
		void stop();

		void pause();
		void resume();
};

}
