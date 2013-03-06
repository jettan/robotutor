#include <utility>
#include <memory>

#include "presentation_commands.hpp"
#include "../engine/script_engine.hpp"
#include "../parser/parser_common.hpp"


namespace robotutor {
	
	namespace {
		/// Parse a relative or absolute offset.
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
			std::cout << "Slide: " << offset << " " << relative << std::endl;
			engine.server.slide(offset, relative);
			engine.current = parent;
			return false;
		}
	}
}

