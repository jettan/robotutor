#include <stdexcept>

#include "core_commands.hpp"
#include "script_engine.hpp"
#include "script_parser.hpp"

namespace robotutor {
	namespace command {
		
		/// Execute one step.
		bool Execute::step(ScriptEngine & engine) {
			if (next < arguments.size()) {
				setNext_(engine, arguments[next++].get());
				return true;
			} else {
				return done_(engine);
			}
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
					setNext_(engine, parent);
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
				setNext_(engine, delayed.front());
				delayed.pop_front();
				return true;
				
			// This command is totally done, we can go to our parent.
			} else {
				return done_(engine);
			}
		}
		
		/// Called when a bookmark is encountered.
		void Speech::onBookmark(ScriptEngine & engine, unsigned int bookmark) {
			setNext_(engine, arguments[bookmark - 1].get());
			continue_(engine);
		}
		
		/// Called when the speech engine finished saying us.
		void Speech::onDone(ScriptEngine & engine, bool interrupted) {
			if (!interrupted) {
				continue_(engine);
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
			engine.stop();
			setNext_(engine, parent);
			return false;
		}
	}
}
