extern "C" {
#include <dlfcn.h>
}

#include <boost/filesystem.hpp>

#include "script_engine.hpp"

namespace robotutor {
	
	namespace {
		typedef bool (*registerPlugin) (ScriptEngine & engine);
	}
	
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
		server.on_message = std::bind(&ScriptEngine::handleMessage_, this, std::placeholders::_1, std::placeholders::_2);
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
	
	/// Add a message handler.
	/**
	 * \param handler The message handler.
	 */
	void ScriptEngine::addMessageHandler(MessageHandler handler) {
		message_handlers_.push_back(handler);
	}
	
	/// Load a plugin from a shared library.
	/**
	 * \param name The name of the shared library.
	 * \return bool True if the plugin was loaded succesfully.
	 */
	bool ScriptEngine::loadPlugin(std::string const & name) {
		void * plugin = dlopen(name.c_str(), RTLD_LAZY);
		if (plugin) {
			registerPlugin register_plugin = reinterpret_cast<registerPlugin>(dlsym(plugin, "registerPlugin"));
			if (register_plugin) return register_plugin(*this);
		}
		return false;
	}
	
	/// Load plugins from a directory.
	/**
	 * All files with a ".so" extension will be loaded as plugins.
	 *
	 * \param directory The directory containing the plugins.
	 * \return The number of succesfully loaded plugins.
	 */
	unsigned int ScriptEngine::loadPlugins(std::string const & directory) {
		boost::filesystem::path path(directory);
		unsigned int total = 0;
		if (boost::filesystem::is_directory(path)) {
			for (boost::filesystem::directory_iterator i(path); i != boost::filesystem::directory_iterator(); ++i) {
				if (i->path().extension() == ".so" && loadPlugin(i->path().native())) ++total;
			}
		}
		return total;
	}
	
	/// Handle messages by passing them to all registered message handlers.
	/**
	 * \param connection The connection that sent the message.
	 * \param message The message.
	 */
	void ScriptEngine::handleMessage_(SharedServerConnection connection, ClientMessage && message) {
		for (auto & handler : message_handlers_) handler(*this, connection, message);
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
	
	/// Run the script.
	void ScriptEngine::continue_() {
		while (current_ && current_->step(*this));
	}
	
}
