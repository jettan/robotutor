#include <string>
#include <stdexcept>
#include <memory>


#include "../plugin.hpp"
#include "../command.hpp"
#include "../script_engine.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to run a behavior.
		struct Behavior : public Command {
			/// The behavior to play.
			std::string behavior;
			
			Behavior(Command * parent, Plugin * plugin, std::string behavior) :
				Command(parent, plugin),
				behavior(behavior) {}
			
			static SharedPtr create(Command * parent, Plugin * plugin, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 argument.");
				return std::make_shared<Behavior>(parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "behavior"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				engine.behavior.enqueue(behavior);
				return done_(engine);
			}
		};
		
		/// Command to run a random.
		struct RandomBehavior : public Command {
			/// The prefix of the random behavior to play.
			std::string prefix;
			
			RandomBehavior(Command * parent, Plugin * plugin, std::string prefix) :
				Command(parent, plugin),
				prefix(prefix) {}
			
			static SharedPtr create(Command * parent, Plugin * plugin, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 argument.");
				return std::make_shared<Behavior>(parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "random behavior"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				engine.behavior.enqueueRandom(prefix);
				return done_(engine);
			}
		};
	}
	
	struct BehaviorPlugin : public Plugin {
		BehaviorPlugin(ScriptEngine & engine) : Plugin(engine) {
			engine.factory.add<command::Behavior>(this);
			engine.factory.add<command::RandomBehavior>(this);
		}
	};
	
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new BehaviorPlugin(engine);
	}
}
