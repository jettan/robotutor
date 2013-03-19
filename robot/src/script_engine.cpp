#include <functional>

#include "script_engine.hpp"


namespace robotutor {
	
	/// Construct the script engine.
	/**
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	ScriptEngine::ScriptEngine(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker) :
		broker(broker),
		speech(SpeechEngine::create(ios, broker, "RTISE")),
		behavior(ios, broker),
		server(ios),
		ios_(ios)
	{
		server.listenIp4(8311);
	}
	
	/// Load a script.
	void ScriptEngine::load(std::shared_ptr<command::Command> script) {
		root_ = script;
		current_ = root_.get();
	}
	
	/// Join any background threads created by the engine.
	/**
	 * Make sure that the IO service has already been stopped,
	 * or it may still process events that use the engine.
	 */
	void ScriptEngine::join() {
		speech->join();
		behavior.join();
		if (wait_thread_.joinable()) wait_thread_.join();
	}
	
	/// Start the engine.
	/**
	 * Does nothing if the engine was already started.
	 */
	void ScriptEngine::start() {
		if (!started_.exchange(true)) continue_();
	}
	
	/// Stop the engine as soon as possible.
	/**
	 * \param callback Callback to invoke when the engine was stopped.
	 */
	void ScriptEngine::stop(std::function<void ()> handler) {
		speech->cancel();
		behavior.drop();
		
		if (wait_thread_.joinable()) wait_thread_.join();
		wait_thread_ = std::thread(std::bind(&ScriptEngine::wait_, this, handler));
	}
	
	/// Run the script.
	void ScriptEngine::continue_() {
		while (current_ && current_->step(*this));
		if (!current_) on_done();
	}
	
	/// Wait for the engine to stop cleanly.
	/**
	 * \param handler The callback to invoke when the waiting is done.
	 */
	void ScriptEngine::wait_(std::function<void ()> handler) {
		speech->join();
		behavior.join();
		started_ = false;
		if (handler) ios_.post(handler);
	}
	
}
