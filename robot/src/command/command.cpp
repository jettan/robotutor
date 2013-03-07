#include "command.hpp"

#include "../engine/script_engine.hpp"
#include "../parser/command_parser.hpp"
#include "../parser/parse.hpp"

namespace robotutor {
	
	namespace command {
		
		/// Write a command to a stream.
		/**
		 * \param stream  The stream to write to.
		 * \param command The command to write.
		 */
		std::ostream & operator << (std::ostream & stream, Command const & command) {
			command.write(stream);
			return stream;
		}
		
		/// Write the command to a stream.
		/**
		 * \param stream The stream to write to.
		 */
		void Command::write(std::ostream & stream) const {
			stream << "{" << name();
			for (auto argument : arguments) stream << "|"  << *argument;
			stream << "}";
		}
		
		/// Should be called when the command is done.
		/**
		 * Sets the current command of the engine to the parent of this command.
		 * 
		 * \param engine The script engine to modify.
		 * \return True.
		 */
		bool Command::done_(ScriptEngine & engine) const {
			engine.current = parent;
			return true;
		}
		
		/// Execute one step.
		bool Execute::step(ScriptEngine & engine) {
			std::cout << "Execute " << this << " ";
			if (next < arguments.size()) {
				std::cout << "step " << next << std::endl;
				engine.current = arguments[next++].get();
			} else {
				std::cout << "done" << std::endl;
				engine.current = parent;
			}
			return true;
		}
		
		/// Create the command.
		SharedPtr Execute::create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory & factory) {
			auto result = std::make_shared<Execute>(parent);
			CommandParser parser(factory);
			for (auto const & argument : arguments) {
				parse(parser, argument);
				result->arguments.push_back(parser.result());
				result->arguments.back()->parent = result.get();
			}
			return result;
		}
		
	}
}
