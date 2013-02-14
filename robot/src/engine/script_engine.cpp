#include "script_engine.hpp"

namespace robotutor {
	
	/// Construct the script engine.
	/**
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	ScriptEngine::ScriptEngine(boost::shared_ptr<AL::ALBroker> broker) :
		speech(SpeechEngine::create(*this, broker, "RTISE")) {}
	
}
