#include <string>
#include <stdexcept>
#include <memory>

#include <alproxies/alaudioplayerproxy.h>

#include "../plugin.hpp"
#include "../command.hpp"
#include "../script_engine.hpp"

namespace robotutor {
	
	struct SoundPlugin : public Plugin {
		/// Behavior manager to use.
		AL::ALAudioPlayerProxy player;

		SoundPlugin(ScriptEngine & engine);
	};

	namespace command {

		struct SoundCommand : public command::Command {
			SoundCommand(ScriptEngine & engine, Command * parent, Plugin * plugin) : Command(engine, parent, plugin) {}
			
			SoundPlugin * sound_plugin() {
				return static_cast<SoundPlugin *>(plugin);
			}
		};
		
		/// Command to run a behavior.
		struct PlaySound : public SoundCommand {
			/// The sound file to play.
			std::string file;
			
			PlaySound(ScriptEngine & engine, Command * parent, Plugin * plugin, std::string file) :
				SoundCommand(engine, parent, plugin),
				file(file) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 argument.");
				return std::make_shared<PlaySound>(engine, parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "sound"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				sound_plugin()->player.playFile(file);
				return done_();
			}
		};
		
		/// Command to run a random.
		struct StopSound : public SoundCommand {
			
			StopSound(ScriptEngine & engine, Command * parent, Plugin * plugin) :
				SoundCommand(engine, parent, plugin) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 0) throw std::runtime_error("Command `" + static_name() + "' expects 0 argument.");
				return std::make_shared<StopSound>(engine, parent, plugin);
			}
			
			static std::string static_name() { return "random behavior"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				sound_plugin()->player.stopAll();
				return done_();
			}
		};
	}

	SoundPlugin::SoundPlugin(ScriptEngine & engine) :
		Plugin(engine),
		player(engine.broker)
	{
		engine.factory.add<command::PlaySound>(this);
		engine.factory.add<command::StopSound>(this);
	}
	
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new SoundPlugin(engine);
	}
}
