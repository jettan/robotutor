#include <stdexcept>

#include "core_commands.hpp"
#include "script_engine.hpp"
#include "script_parser.hpp"

namespace robotutor {
	namespace command {
		
		/// Execute one step.
		bool Execute::step() {
			if (next < children.size()) {
				setNext_(children[next++].get());
				return true;
			} else {
				return done_();
			}
		}
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Speech::write(std::ostream & stream) const {
			stream << text;
			for (auto & child : children) stream << "\n" << *child;
		}
		
		/// Create the command.
		SharedPtr Execute::create(ScriptEngine & engine, Command * parent, Plugin *, std::vector<std::string> && arguments) {
			auto result = std::make_shared<Execute>(engine, parent);
			for (auto const & argument : arguments) {
				result->children.push_back(parseScript(engine, argument));
				result->children.back()->parent = result.get();
			}
			return result;
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 * \return True if an asynchronous operation was started.
		 */
		bool Speech::step() {
			if (!synthesized) {
				// The command hasn't been synthesised yet, but the engine is busy.
				// The command has to be delayed.
				if (engine.speech->job()) {
					engine.speech->job()->command->delayed.push_back(this);
					setNext_(parent);
					return true;
					
				// The engine isn't busy, the command should be synthesized.
				} else {
					auto on_bookmark = std::bind(&Speech::onBookmark, this, std::placeholders::_1);
					auto on_done     = std::bind(&Speech::onDone    , this, std::placeholders::_1);
					engine.speech->say(*this, on_bookmark, on_done);
					synthesized = true;
					return false;
				}
				
			// This command is the current job of the TTS engine, so wait for it to finish.
			} else if (engine.speech->job() && engine.speech->job()->command == this) {
				return false;
				
			// There are delayed commands to execute.
			} else if (delayed.size()) {
				setNext_(delayed.front());
				delayed.pop_front();
				return true;
				
			// This command is totally done, we can go to our parent.
			} else {
				return done_();
			}
		}
		
		/// Called when a bookmark is encountered.
		void Speech::onBookmark(unsigned int bookmark) {
			setNext_(children[bookmark - 1].get());
			continue_();
		}
		
		/// Called when the speech engine finished saying us.
		void Speech::onDone(bool interrupted) {
			if (!interrupted) continue_();
		}
		
		/// Create a stop command.
		SharedPtr Stop::create(ScriptEngine & engine, Command * parent, Plugin *, std::vector<std::string> && arguments) {
			if (arguments.size()) throw std::runtime_error("Command `" + static_name() + "' takes zero arguments.");
			return std::make_shared<Stop>(engine, parent);
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Stop::step() {
			engine.stop();
			setNext_(parent);
			return false;
		}
	}
}
