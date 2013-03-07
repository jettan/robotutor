#include "script_engine.hpp"

using namespace boost::asio;
namespace robotutor {
	
	/// Construct the script engine.
	/**
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	ScriptEngine::ScriptEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker) :
		speech(SpeechEngine::create(*this, ios, broker, "RTISE")),
		behavior(*this, ios, broker),
		server(*this, ios),
		current(nullptr)
	{
		server.listen(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8311));
	}
	
	/// Join any background threads created by the engine.
	/**
	 * Make sure that the IO service has already been stopped,
	 * or it may still process events that use the engine.
	 */
	void ScriptEngine::join() {
		speech->join();
	}
	
	/// Run the script.
	void ScriptEngine::run() {
		while (current && current->step(*this));
	}
	
}
