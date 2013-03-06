#include <ostream>

#include "speech_commands.hpp"
#include "../engine/script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Speech::write(std::ostream & stream) const {
			stream << text;
			for (auto & argument : arguments) stream << "\n" << *argument;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 * \return True if an asynchronous operation was started.
		 */
		bool Speech::step(ScriptEngine & engine) {
			if (!synthesized) {
				// The command hasn't been synthesised yet, but the engine is busy.
				// The command has to be delayed.
				if (engine.speech->job()) {
					engine.speech->job()->command->delayed.push_back(this);
					engine.current = parent;
					return false;
					
				// The engine isn't busy, the command should be synthesized.
				} else {
					auto on_bookmark = std::bind(&Speech::onBookmark, this, std::ref(engine), std::placeholders::_1);
					auto on_done     = std::bind(&Speech::onDone    , this, std::ref(engine), std::placeholders::_1);
					engine.speech->say(*this, on_bookmark, on_done);
					synthesized = true;
					return true;
				}
				
			// This command is the current job of the TTS engine, so wait for it to finish.
			} else if (engine.speech->job() && engine.speech->job()->command == this) {
				return true;
				
			// There are delayed commands to execute.
			} else if (delayed.size()) {
				engine.current = delayed.front();
				delayed.pop_front();
				return false;
				
			// This command is totally done, we can go to our parent.
			} else {
				std::cout << "Speech done: " << parent << std::endl;
				engine.current = parent;
				return false;
			}
		}
		
		void Speech::onBookmark(ScriptEngine & engine, unsigned int bookmark) {
			engine.current = arguments[bookmark - 1].get();
			engine.run();
		}
		
		void Speech::onDone(ScriptEngine & engine, bool interrupted) {
			if (!interrupted) {
				engine.run();
			}
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Stop::step(ScriptEngine & engine) {
			engine.speech->cancel();
			engine.current = parent;
			return false;
		}
	}
}
