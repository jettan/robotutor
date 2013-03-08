#include <utility>
#include <memory>

#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "presentation_commands.hpp"
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
				if (!parser::isDigit(*i)) throw std::runtime_error("Unexpected character encountered slide command argument.");
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
		
		/// Construct a slide command.
		/**
		 * \param parent The parent command.
		 * \param offset The offset.
		 * \param If true, the offset is relative.
		 */
		Slide::Slide(Command * parent, int offset, bool relative) :
			Command(parent),
			offset(offset),
			relative(relative) {}
		
		/// Create the command.
		SharedPtr Slide::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
			if (arguments.size() == 0) {
				return std::make_shared<Slide>(parent, 1, true);
			} else if (arguments.size() == 1) {
				std::pair<int, bool> offset = parseOffset(arguments[0]);
				return std::make_shared<Slide>(parent, offset.first, offset.second);
			} else {
				throw std::runtime_error("Too many arguments given for slide command. Expected 0 or 1 arguments.");
			}
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool Slide::step(ScriptEngine & engine) {
			RobotMessage message;
			message.mutable_slide()->set_offset(offset);
			message.mutable_slide()->set_relative(relative);
			engine.server.sendMessage(message);
			
			return done_(engine);
		}
		
		
		/// Construct a ShowImage command.
		/**
		 * \param parent The parent command.
		 */
		ShowImage::ShowImage(Command * parent) :
			Command(parent) {}
		
		/// Create the command.
		SharedPtr ShowImage::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
			if (arguments.size() == 0) {
				return std::make_shared<ShowImage>(parent);
			} else {
				throw std::runtime_error("Too many arguments given for show image command. Expected 0.");
			}
		}
		
		/// Run the command.
		/**
		 * \param engine The script engine to use for executing the command.
		 */
		bool ShowImage::step(ScriptEngine & engine) {
			grabImage(engine.broker);
			
			RobotMessage msg;
			msg.mutable_show_image();
			engine.server.sendMessage(msg);
			
			return done_(engine);
		}
	}
}

