#include "script_engine.hpp"

using namespace boost::asio;
namespace robotutor {
	
	/// Construct the script engine.
	/**
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	ScriptEngine::ScriptEngine(boost::shared_ptr<AL::ALBroker> broker, boost::asio::io_service & ios) :
		speech(SpeechEngine::create(*this, broker, "RTISE")),
		behavior(broker, ios),
		server(ios, ip::tcp::endpoint(ip::tcp::v4(), 8311))
	{
		server.start();
	}
	
}
