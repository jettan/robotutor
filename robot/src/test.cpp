#include <iostream>
#include <memory>

#include "command/command.hpp"
#include "parser/parse.hpp"
#include "parser/command_parser.hpp"


namespace robotutor {
	
	namespace command {
		
		struct Generic : public Command {
			
			std::string const name_;
			
			Generic(std::string const & name, ArgList const & arguments) :
				Command(arguments),
				name_(name) {}
			
			Generic(std::string && name, ArgList && arguments) :
				Command(std::move(arguments)),
				name_(std::move(name)) {}
			
			static SharedPtr create(std::string const & name, ArgList const & arguments) {
				return std::make_shared<Generic>(name, arguments);
			}
			
			std::string const name() const {
				return name_;
			}
			
			bool run(ScriptEngine &) const {
				return true;
			}
		};
		
	}
	

}

using namespace robotutor;

int main() {
	command::Factory commands;
	commands.add("aap", command::Generic::create);
	commands.add("mies", command::Generic::create);
	robotutor::CommandParser parser(commands);
	
	try {
		robotutor::parse(parser, std::cin);
		command::SharedPtr script = parser.result();
		std::cout << *script;
	} catch (std::exception const & e) {
		std::cerr << "Error during parsing: " << e.what() << "." << std::endl;
	}
}
