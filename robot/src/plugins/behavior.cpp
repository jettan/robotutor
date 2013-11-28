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
			
			Behavior(ScriptEngine & engine, Command * parent, Plugin * plugin, std::string behavior) :
				Command(engine, parent, plugin),
				behavior(behavior) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 argument.");
				return std::make_shared<Behavior>(engine, parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "behavior"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				//engine.behavior.enqueue(behavior);
				RobotMessage message;
				message.mutable_behaviorcmd()->set_behaviorname(behavior);
				engine.server.sendMessage(message);
				return done_();
			}
		};
		
		/// Command to run a random.
		struct RandomBehavior : public Command {
			/// The prefix of the random behavior to play.
			std::string prefix;
			
			RandomBehavior(ScriptEngine & engine, Command * parent, Plugin * plugin, std::string prefix) :
				Command(engine, parent, plugin),
				prefix(prefix) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 argument.");
				return std::make_shared<Behavior>(engine, parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "random behavior"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				engine.behavior.enqueueRandom(prefix);
				return done_();
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
