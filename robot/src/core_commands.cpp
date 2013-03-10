#include <stdexcept>

#include "core_commands.hpp"
#include "script_engine.hpp"
#include "script_parser.hpp"

namespace robotutor {
	namespace command {
		
		/// Execute one step.
		bool Execute::step(ScriptEngine & engine) {
			std::cout << "Execute " << this << " ";
			if (next < arguments.size()) {
				std::cout << "step " << next << std::endl;
				engine.current = arguments[next++].get();
			} else {
				std::cout << "done" << std::endl;
				engine.current = parent;
			}
			return true;
		}
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Speech::write(std::ostream & stream) const {
			stream << text;
			for (auto & argument : arguments) stream << "\n" << *argument;
		}
		
		/// Create the command.
		SharedPtr Execute::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory & factory) {
			auto result = std::make_shared<Execute>(parent);
			for (auto const & argument : arguments) {
				result->arguments.push_back(parseScript(factory, argument));
				result->arguments.back()->parent = result.get();
			}
			return result;
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
					return true;
					
				// The engine isn't busy, the command should be synthesized.
				} else {
					auto on_bookmark = std::bind(&Speech::onBookmark, this, std::ref(engine), std::placeholders::_1);
					auto on_done     = std::bind(&Speech::onDone    , this, std::ref(engine), std::placeholders::_1);
					engine.speech->say(*this, on_bookmark, on_done);
					synthesized = true;
					return false;
				}
				
			// This command is the current job of the TTS engine, so wait for it to finish.
			} else if (engine.speech->job() && engine.speech->job()->command == this) {
				return false;
				
			// There are delayed commands to execute.
			} else if (delayed.size()) {
				engine.current = delayed.front();
				delayed.pop_front();
				return true;
				
			// This command is totally done, we can go to our parent.
			} else {
				std::cout << "Speech done: " << parent << std::endl;
				return done_(engine);
			}
		}
		
		/// Called when a bookmark is encountered.
		void Speech::onBookmark(ScriptEngine & engine, unsigned int bookmark) {
			engine.current = arguments[bookmark - 1].get();
			engine.run();
		}
		
		/// Called when the speech engine finished saying us.
		void Speech::onDone(ScriptEngine & engine, bool interrupted) {
			if (!interrupted) {
				engine.run();
			}
		}
		
		/// Create a stop command.
		SharedPtr Stop::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
			if (arguments.size()) throw std::runtime_error("Stop command takes zero arguments.");
			return std::make_shared<Stop>(parent);
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Stop::step(ScriptEngine & engine) {
			engine.speech->cancel();
			return done_(engine);
		}
	}
}
