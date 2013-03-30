#include <string>
#include <memory>

#include "robotutor_protocol.hpp"


namespace robotutor {
	
	class ScriptEngine;
	
	namespace command {
		class Factory;
	}
	
	/// Base class for script engine plugins.
	class Plugin {
		friend class command::Factory;
		
		/// Signature of function that must be exported by plugins.
		typedef Plugin * (*createPlugin) (ScriptEngine & engine);
		
		private:
			/// The handle returned by dlopen.
			void * handle_ = nullptr;
			
		public:
			/// The script engine.
			ScriptEngine & engine;
			
			/// Construct a plugin.
			/**
			 * \param engine The script engine.
			 */
			Plugin(ScriptEngine & engine) :
				engine(engine) {}
			
			Plugin            (Plugin const &) = delete;
			Plugin & operator=(Plugin const &) = delete;
			
			/// Descontruct a plugin.
			/**
			 * Closes the handle using dlclose().
			 */
			virtual ~Plugin();
			
			/// Create a plugin from a file.
			/**
			 * \param file The file containing the plugin.
			 * \param engine The script engine the plugin is for.
			 */
			static std::shared_ptr<Plugin> load(std::string const & file, ScriptEngine & engine);
			
			/// Called by the engine when it starts.
			virtual void start() {}
			
			/// Called by the engine when it stops.
			virtual void stop() {}
			
			/// Join any threads created by the plugin.
			virtual void join() {}
			
			/// Process server messages.
			/**
			 * \param connection The connection that sent the message.
			 * \param message The message.
			 */
			virtual void handleMessage(SharedServerConnection connection, ClientMessage const & message) {}
	};
	
}
