#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <alcommon/almodule.h>
#include <alproxies/altexttospeechproxy.h>

#include "../executable.hpp"

namespace AL {
	class ALBroker;
	class ALMemoryProxy;
}

namespace robotutor {
	
	/// Speech engine context.
	struct SpeechContext {
		
		/// Construct a speech engine context.
		SpeechContext(executable::Text::SharedPtr text, int job_id)
			: text(text)
			, last_word(0)
			, last_sentence(0)
			, job_id(job_id)
			{}
		
		/// The text belonging to this context.
		executable::Text::SharedPtr text;
		/// The position of the last word.
		unsigned int last_word;
		/// The position of the last sentence.
		unsigned int last_sentence;
		/// Job id with the speech engine.
		int job_id;
	};
	
	/// Speech engine to execute executable::Text.
	class SpeechEngine : public AL::ALModule {
		public:
			/// Callback type for the command callback.
			typedef boost::function<void (executable::SharedPtr)> Callback;
			
			/// The callback to execute when a command is encountered.
			Callback command_callback;
			
		protected:
			/// Memory proxy to receive callbacks.
			boost::shared_ptr<AL::ALMemoryProxy> memory_;
			
			/// TTS proxy to do the actual synthesizing.
			AL::ALTextToSpeechProxy tts_;
			
			/// Vector containing job IDs
			std::vector<SpeechContext> stack_;
			
		public:
			/// Construct the speech engine.
			SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstruct the speech engine.
			virtual ~SpeechEngine();
			
			/// Initialize the naoqi module.
			virtual void init();
			
			/// Execute a text command.
			/**
			 * The previous command is interrupted and will be resumed when this one finishes.
			 * \param text The text to execute.
			 */
			void execute(executable::Text::SharedPtr text);
			
			/// Stop execution.
			void stop();
			
			/// Resume execution.
			void resume();
			
			/// Reset the engine.
			void reset();
			
		protected:
			/// Resume a speech context.
			/**
			 * \param context The context to resume.
			 */
			void resumeContext_(SpeechContext & context);
			
		public:
			/// Called when a bookmark is encountered.
			void onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
			/// Called when word is finished.
			void onWordPos(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
	};
}
