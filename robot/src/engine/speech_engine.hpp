#pragma once

#include <string>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>

#include <boost/signal.hpp>

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
	class SpeechContext {
		friend class SpeechEngine;
		
		protected:
			/// The parent script engine.
			ScriptEngine & parent;
			
			/// The interrupted context, or null if there is none.
			SpeechContext * interrupted;
			
			/// The text belonging to this context.
			command::Text::SharedPtr text;
			
			/// Last executed mark.
			int mark = 0;
			
			/// Index of the most recently played sentence.
			int sentence = 0;
			
			/// Interruption queue for the current context.
			std::deque<std::shared_ptr<SpeechContext>> interrupts;
			
		public:
			/// Construct a speech engine context.
			SpeechContext(ScriptEngine & parent, command::Text::SharedPtr text, SpeechContext * interrupted) :
				parent(parent),
				interrupted(interrupted),
				text(text) {}
				
		protected:
			/// Perform the next step of the context.
			/**
			 * If this method returns true, the next step should wait
			 * until the started aynchronous operation completed.
			 * 
			 * \return True if the step started an asynchronous operation.
			 */
			bool step();
			
			/// Execute all unexecuted commands up to the given index.
			/**
			 * \param command The index.
			 */
			void executeCommand(int command);
	};
	
	/// Speech engine to execute command::Text.
	class SpeechEngine : public AL::ALModule {
		friend class SpeechContext;
		
		public:
			/// Signal indicating that the engine is done.
			boost::signal<void (SpeechEngine & engine)> on_done;
			
			/// Signal indicating that the engine has paused.
			boost::signal<void (SpeechEngine & engine)> on_pause;
			
			/// Signal indicating that the engine has resumed.
			boost::signal<void (SpeechEngine & engine)> on_resume;
			
		protected:
			
			/// Reference to the parent script engine.
			ScriptEngine * parent_;
			
			/// Text queue.
			std::deque<SpeechContext> queue_;
			
			/// Current context.
			SpeechContext * current_ = nullptr;
			
			/// Job ID of the currently running task.
			int job_id_ = 0;
			
			/// If true, the engine is processing the queue.
			bool playing_ = false;
			
			/// Memory proxy to receive callbacks.
			boost::shared_ptr<AL::ALMemoryProxy> memory_;
			
			/// TTS proxy to do the actual synthesizing.
			AL::ALTextToSpeechProxy tts_;
			
			/// Mutex to lock when processing callbacks.
			std::recursive_mutex mutex_;
			
		public:
			/// Construct the speech engine.
			SpeechEngine(boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstruct the speech engine.
			virtual ~SpeechEngine();
			
			/// Create a speech engine.
			static boost::shared_ptr<SpeechEngine> create(ScriptEngine & parent, boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Execute a text command by interrupting the current text.
			/**
			 * Note that the current text will be interrupted at the next sentence end.
			 * 
			 * \param text The text command to execute.
			 */
			void interrupt(command::Text::SharedPtr text);
			
			/// Execute a text command.
			/**
			 * The text command will be queued and executed when the last of the currently remaining job is done.
			 * 
			 * \param text The text command to execute.
			 */
			void enqueue(command::Text::SharedPtr text);
			
			/// Initialize the module.
			void init();
			
			/// Pause execution at the next sentence end.
			void pause();
			
			/// Resume execution.
			void resume();
			
			/// Reset the engine.
			void reset();
			
			/// Called when a bookmark is encountered.
			void onBookmark(std::string const & eventName, int const & value, std::string const & subscriberIndentifier);
			
			/// Called when the TTS engine is done.
			void onTextDone(std::string const & eventName, bool const & value, std::string const & subsciberIdentifier);
			
		protected:
			/// Step trough the contexts.
			void step_();
				
			/// Say a text.
			/**
			 * \param text The text.
			 */
			void say_(std::string const & text);
	};
}
