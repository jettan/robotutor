#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <boost/asio/system_timer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "../command.hpp"
#include "../plugin.hpp"
#include "../script_engine.hpp"


namespace robotutor {
	
	class PoseChanger {
		public:
			/// The prefix to run random behaviors from.
			std::string prefix;
			
			/// The minimum time in milliseconds to wait between behaviors.
			unsigned int min;
			
			/// The maximum time in milliseconds to wait between behaviors.
			unsigned int max;
			
		protected:
			/// The script engine to control.
			ScriptEngine & engine_;
			
			/// Timer to wait random periods.
			boost::asio::system_timer timer_;
			
			/// True if the engine was started.
			std::atomic_flag started_;
			
		public:
			/// Create a pose changer.
			/**
			 * \param engine The script engine to control.
			 * \param prefix The prefix to select behaviors from.
			 * \param min The minimum time in milliseconds between two random poses.
			 * \param max The maximum time in milliseconds between two random poses.
			 */
			PoseChanger(ScriptEngine & engine, std::string prefix, unsigned int min = 1000, unsigned int max = 4000) :
				prefix(prefix),
				min(min),
				max(max),
				engine_(engine),
				timer_(engine_.ios())
			{
				started_.clear();
			}
			
			/// Start executing random behaviors.
			void start() {
				asyncWaitRandom_();
			}
			
			/// Stop executing random behaviors.
			void cancel() {
				timer_.cancel();
				started_.clear();
			}
			
		protected:
			/// Wait a random period before calling the timeout function.
			void asyncWaitRandom_() {
				if (!started_.test_and_set()) {
					boost::random::uniform_int_distribution<unsigned int> distribution(min, max);
					unsigned int timeout = distribution(engine_.random);
					timer_.expires_from_now(std::chrono::milliseconds(timeout));
					timer_.async_wait(std::bind(&PoseChanger::handleTimeout_, this, std::placeholders::_1));
				}
			}
			
			/// Handle a timeout.
			/**
			 * \param error The error that occured, if any.
			 */
			void handleTimeout_(boost::system::error_code const & error) {
				if (!error) {
					std::cout << "Executing random behavior." << std::endl;
					if (!engine_.behavior.queued()) engine_.behavior.enqueueRandom(prefix);
					asyncWaitRandom_();
				} else {
					started_.clear();
				}
			}
	};
	
	struct PoseChangerPlugin : public Plugin {
		PoseChanger pose_changer;
		
		PoseChangerPlugin(ScriptEngine & engine);
		
		void stop() override {
			pose_changer.cancel();
		}
	};
	
	namespace command {
		
		/// Command to enable random pose changes.
		struct EnablePoseChanger : public Command {
			EnablePoseChanger(ScriptEngine & engine, Command * parent, Plugin * plugin) :
				Command(engine, parent, plugin) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 0) throw std::runtime_error("Command `" + static_name() + "' expects 0 arguments.");
				return std::make_shared<EnablePoseChanger>(engine, parent, plugin);
			}
			
			static std::string static_name() { return "enable pose changer"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				static_cast<PoseChangerPlugin *>(plugin)->pose_changer.start();
				return done_();
			}
		};
		
		/// Command to disable random pose changes.
		struct DisablePoseChanger : public Command {
			DisablePoseChanger(ScriptEngine & engine, Command * parent, Plugin * plugin) :
				Command(engine, parent, plugin) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 0) throw std::runtime_error("Command `" + static_name() + "' expects 0 arguments.");
				return std::make_shared<DisablePoseChanger>(engine, parent, plugin);
			}
			
			static std::string static_name() { return "disable pose changer"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				static_cast<PoseChangerPlugin *>(plugin)->pose_changer.cancel();
				return done_();
			}
		};
		
		/// Command to set random pose prefix.
		struct PoseChangerPrefix : public Command {
			std::string prefix;
			
			PoseChangerPrefix(ScriptEngine & engine, Command * parent, Plugin * plugin, std::string const & prefix) :
				Command(engine, parent, plugin),
				prefix(prefix) {}
			
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() != 1) throw std::runtime_error("Command `" + static_name() + "' expects 1 arguments.");
				return std::make_shared<PoseChangerPrefix>(engine, parent, plugin, arguments[0]);
			}
			
			static std::string static_name() { return "pose prefix"; }
			
			std::string name() const { return static_name(); }
			
			bool step() {
				static_cast<PoseChangerPlugin *>(plugin)->pose_changer.prefix = prefix;
				return done_();
			}
		};
	}
	
	PoseChangerPlugin::PoseChangerPlugin(ScriptEngine & engine) :
		Plugin(engine),
		pose_changer(engine, "short")
	{
		engine.factory.add<command::EnablePoseChanger>(this);
		engine.factory.add<command::DisablePoseChanger>(this);
		engine.factory.add<command::PoseChangerPrefix>(this);
	}
	
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new PoseChangerPlugin(engine);
	}
}
