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
		server(ios, ip::tcp::endpoint(ip::tcp::v4(), 8311))
	{
		server.start();
	}
	
	/// Join any background threads created by the engine.
	/**
	 * Make sure that the IO service has already been stopped,
	 * or it may still process events that use the engine.
	 */
	void ScriptEngine::join() {
		speech->join();
	}
	
}
