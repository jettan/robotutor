#include <utility>
#include <memory>

#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../plugin.hpp"
#include "../command.hpp"
#include "../script_engine.hpp"
#include "../parser_common.hpp"

namespace robotutor {
	
	namespace {
		/// Parse a relative or absolute offset.
		/**
		 * A string starting with + or - is a relative offset.
		 * Anything else is absolute.
		 * The string may start with '+' or '-' and must otherwise consist only of digits.
		 * 
		 * \param input The input string.
		 */
		std::pair<int, bool> parseOffset(std::string input) {
			parser::trim(input);
			if (!input.length()) return std::make_pair(1, true);
			std::pair<int, bool> result;
			
			// First character can be '-' or '+' to indicate a relative offset.
			auto i = input.begin();
			if (*i == '-' || *i == '+') {
				result.second = true;
				++i;
			}
			
			// All remaining characters should be numerical.
			for (; i != input.end(); ++i) {
				if (!parser::isDigit(*i)) throw std::runtime_error("Unexpected character encountered in offset.");
				result.first = result.first * 10 + *i - '0';
			}
			if (input[0] == '-') result.first *= -1;
			
			
			return result;
		}
		
		/// Grab an image from the NAO camera.
		/**
		 * The image is saved to "/var/www/capture.jpg".
		 * \param broker The broker to communicate with naoqi.
		 */
		void grabImage(boost::shared_ptr<AL::ALBroker> broker) {
			
			// The camera proxy.
			AL::ALVideoDeviceProxy camera_proxy(broker);
			
			// Subscribe a client image requiring 320*240 and BGR colorspace.
			const std::string client_name = camera_proxy.subscribeCamera("RoboTutorCamera", 0, AL::k4VGA, AL::kBGRColorSpace, 5);
			cv::Mat image                 = cv::Mat(cv::Size(1280, 960), CV_8UC3);
			AL::ALValue al_image          = camera_proxy.getImageRemote(client_name);
			image.data                    = (uchar*) al_image[6].GetBinary();
			
			camera_proxy.releaseImage(client_name);
			cv::imwrite("/var/www/capture.jpg", image);
			camera_proxy.unsubscribe(client_name);
		}
	}
	
	namespace command {
		
		/// Command to go to a different slide.
		struct Slide : public Command {
			
			/// The offset.
			int offset;
			
			/// If true, the offset is relative to the current slide.
			/// If false, the offset is relative to slide 0.
			bool relative;
			
			Slide(Command * parent, Plugin * plugin, int offset, bool relative) :
				Command(parent, plugin),
				offset(offset),
				relative(relative) {}
			
			static std::string static_name() { return "slide"; }
			
			std::string name() const { return static_name(); }
			
			static SharedPtr create(Command * parent, Plugin * plugin, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() == 0) {
					return std::make_shared<Slide>(parent, plugin, 1, true);
				} else if (arguments.size() == 1) {
					std::pair<int, bool> offset = parseOffset(arguments[0]);
					return std::make_shared<Slide>(parent, plugin, offset.first, offset.second);
				} else {
					throw std::runtime_error("Command `" + static_name() + "' expects 0 or 1 arguments.");
				}
			}
			
			bool step(ScriptEngine & engine) {
				RobotMessage message;
				message.mutable_slide()->set_offset(offset);
				message.mutable_slide()->set_relative(relative);
				engine.server.sendMessage(message);
				
				return done_(engine);
			}
		};
		
		
		struct ShowImage : public Command {
			/// Construct a show image command.
			/**
			 * \param parent The parent command.
			 * \param plugin Plugin that created us.
			 */
			ShowImage(Command * parent, Plugin * plugin) :
				Command(parent, plugin) {}
			
			static SharedPtr create(Command * parent, Plugin * plugin, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() == 0) {
					return std::make_shared<ShowImage>(parent, plugin);
				} else {
					throw std::runtime_error("Command `" + static_name() + "' expects 0 arguments.");
				}
			}
			
			static std::string static_name() { return "show image"; }
			
			std::string name() const { return static_name(); }
			
			bool step(ScriptEngine & engine) {
				grabImage(engine.broker);
				
				RobotMessage msg;
				msg.mutable_show_image();
				engine.server.sendMessage(msg);
				
				return done_(engine);
			}
		};
	}
	
	struct PresentationPlugin : public Plugin {
		PresentationPlugin(ScriptEngine & engine) : Plugin(engine) {
			engine.factory.add<command::Slide>(this);
			engine.factory.add<command::ShowImage>(this);
		}
	};
	
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new PresentationPlugin(engine);
	}
}

