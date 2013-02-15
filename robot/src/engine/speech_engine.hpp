#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>

#include <alcommon/almodule.h>
#include <alproxies/altexttospeechproxy.h>

#include "../command/speech_commands.hpp"

namespace AL {
	class ALBroker;
	class ALMemoryProxy;
}

namespace robotutor {
	
	class ScriptEngine;
	
	/// Speech engine context.
	struct SpeechContext {
		/// Construct a speech engine context.
		SpeechContext(std::shared_ptr<command::Text const> text) :
			text(text),
			start(0),
			last_word(0) {}
		
		/// The text belonging to this context.
		std::shared_ptr<command::Text const> text;
		/// The position where the syntheses started.
		unsigned int start;
		/// The position of the last word.
		unsigned int last_word;
	};
	
	/// Speech engine to execute command::Text.
	class SpeechEngine : public AL::ALModule {
		
		public:
			/// Callback type for the command callback.
			typedef std::function<void (command::SharedPtr)> Callback;
			
			/// The callback to execute when a command is encountered.
			Callback command_callback;
			
			/// Stack of speech contexts.
			std::vector<SpeechContext> stack;
			
			/// Job ID of the currently running task.
			int job_id_;
			
		protected:
			/// Reference to the parent script engine.
			ScriptEngine * parent_;
			
			/// Memory proxy to receive callbacks.
			boost::shared_ptr<AL::ALMemoryProxy> memory_;
			
			/// TTS proxy to do the actual synthesizing.
			AL::ALTextToSpeechProxy tts_;
			
			/// If true, the TTS engine is busy.
			bool playing_;
			
			/// Mutex to lock when processing callbacks.
			std::mutex callback_mutex;
			
		public:
			/// Construct the speech engine.
			SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstruct the speech engine.
			virtual ~SpeechEngine();
			
			/// Create a speech engine.
			static boost::shared_ptr<SpeechEngine> create(ScriptEngine & parent, boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Execute a text command.
			/**
			 * The previous command is interrupted and will be resumed when this one finishes.
			 * \param text The text to execute.
			 */
			void executeCommand(std::shared_ptr<command::Text const> text);
			
			/// Initialize the module.
			void init();
			
			/// Stop execution.
			void stopCommand();
			
			/// Resume execution.
			void resumeCommand();
			
			/// Reset the engine.
			void reset();
			
			/// Called when a bookmark is encountered.
			void onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
			/// Called when word is finished.
			void onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
			/// Called when the TTS engine is done.
			void onTextDone(std::string const & eventName, bool const & value, std::string const & subsciberIdentifier);
	};
}
