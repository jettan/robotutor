#include <string>
#include <stdexcept>
#include <memory>

#include "../script_engine.hpp"
#include "../command.hpp"

namespace robotutor {
	namespace command {
		
		/// Command to enable random pose changes.
		struct EnablePoseChanges : public Command {
			EnablePoseChanges(Command * parent) :
				Command(parent) {}
			
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 0) throw std::runtime_error("Command `" + name + "' expects 0 arguments.");
				return std::make_shared<EnablePoseChanges>(parent);
			}
			
			static std::string static_name() { return "enable pose changes"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				engine.pose_changer.start();
				return done_(engine);
			}
		};
		
		/// Command to disable random pose changes.
		struct DisablePoseChanges : public Command {
			DisablePoseChanges(Command * parent) :
				Command(parent) {}
			
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 0) throw std::runtime_error("Command `" + name + "' expects 0 arguments.");
				return std::make_shared<DisablePoseChanges>(parent);
			}
			
			static std::string static_name() { return "disable pose changes"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				engine.pose_changer.cancel();
				return done_(engine);
			}
		};
		
		/// Command to set random pose prefix.
		struct PoseChangesPrefix : public Command {
			std::string prefix;
			
			PoseChangesPrefix(Command * parent, std::string const & prefix) :
				Command(parent),
				prefix(prefix) {}
			
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + name + "' expects 1 arguments.");
				return std::make_shared<PoseChangesPrefix>(parent, arguments[0]);
			}
			
			static std::string static_name() { return "pose changes prefix"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				engine.pose_changer.prefix = prefix;
				return done_(engine);
			}
		};
		
		
		/// Register the plugin with a script engine.
		/**
		 * \param engine The script engine.
		 */
		extern "C" bool registerPlugin(ScriptEngine & engine) {
			engine.factory.add<EnablePoseChanges>();
			engine.factory.add<DisablePoseChanges>();
			engine.factory.add<PoseChangesPrefix>();
			return true;
		}
	}
}
