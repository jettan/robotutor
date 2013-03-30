#include <string>
#include <stdexcept>
#include <memory>

#include "../script_engine.hpp"
#include "../command.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to run a behavior.
		struct Behavior : public Command {
			/// The behavior to play.
			std::string behavior;
			
			/// Construct the command.
			/**
			 * \param arguments The arguments for the command.
			 */
			Behavior(Command * parent, std::string behavior) :
				Command(parent),
				behavior(behavior) {}
			
			/// Create the command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Behavior command expects 1 argument.");
				return std::make_shared<Behavior>(parent, arguments[0]);
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			static std::string static_name() { return "behavior"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return Behavior::static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine) {
				engine.behavior.enqueue(behavior);
				return done_(engine);
			}
		};
		
		/// Command to run a random.
		struct RandomBehavior : public Command {
			/// The behavior to play.
			std::string prefix;
			
			/// Construct the command.
			/**
			 * \param arguments The arguments for the command.
			 */
			RandomBehavior(Command * parent, std::string prefix) :
				Command(parent),
				prefix(prefix) {}
			
			/// Create the command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Random behavior command expects 1 argument.");
				return std::make_shared<Behavior>(parent, arguments[0]);
			}
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			static std::string static_name() { return "random behavior"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return this->static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine) {
				engine.behavior.enqueueRandom(prefix);
				return done_(engine);
			}
		};
		
		/// Register the plugin with a script engine.
		/**
		 * \param engine The script engine.
		 */
		extern "C" bool registerPlugin(ScriptEngine & engine) {
			engine.factory.add<Behavior>();
			return true;
		}
	}
}
