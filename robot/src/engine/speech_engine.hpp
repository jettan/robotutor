#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <alcommon/almodule.h>
#include <alproxies/altexttospeechproxy.h>

#include "../command/text_command.hpp"

namespace AL {
	class ALBroker;
	class ALMemoryProxy;
}

namespace robotutor {
	
	class ScriptEngine;
	
	/// Speech engine context.
	struct SpeechContext {
		/// Construct a speech engine context.
		SpeechContext(command::Text::SharedPtr text) :
			text(text),
			start(0),
			last_word(0) {}
		
		/// The text belonging to this context.
		command::Text::SharedPtr text;
		/// The position where the syntheses started.
		unsigned int start;
		/// The position of the last word.
		unsigned int last_word;
	};
	
	/// Speech engine to execute command::Text.
	class SpeechEngine : public AL::ALModule {
		friend class ScriptEngine;
		
		public:
			/// Callback type for the command callback.
			typedef boost::function<void (command::SharedPtr)> Callback;
			
			/// The callback to execute when a command is encountered.
			Callback command_callback;
			
			/// Stack of speech contexts.
			std::vector<SpeechContext> stack;
			
		protected:
			/// Reference to the parent script engine.
			ScriptEngine & parent_;
			
			/// Memory proxy to receive callbacks.
			boost::shared_ptr<AL::ALMemoryProxy> memory_;
			
			/// TTS proxy to do the actual synthesizing.
			AL::ALTextToSpeechProxy tts_;
			
		public:
			/// Construct the speech engine.
			SpeechEngine(ScriptEngine & parent, boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstruct the speech engine.
			virtual ~SpeechEngine();
			
			/// Execute a text command.
			/**
			 * The previous command is interrupted and will be resumed when this one finishes.
			 * \param text The text to execute.
			 */
			void executeCommand(command::Text::SharedPtr text);
			
			/// Stop execution.
			void stopCommand();
			
			/// Resume execution.
			void resumeCommand();
			
			/// Reset the engine.
			void reset();
			
		public:
			/// Called when a bookmark is encountered.
			void onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
			/// Called when word is finished.
			void onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
	};
}
