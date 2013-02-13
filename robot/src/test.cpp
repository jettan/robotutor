#include <iostream>
#include <boost/make_shared.hpp>

#include "command/command.hpp"
#include "parser/script_parser.hpp"


namespace robotutor {
	
	namespace command {
		
		struct Generic : public Command {
			
			std::string name_;
			
			static SharedPtr create(std::string const & name, ArgList const & arguments) {
				auto result  = boost::make_shared<Generic>();
				result->name_     = name;
				result->arguments = arguments;
				return result;
			}
			
			std::string const name() const {
				return name_;
			}
			
			bool run(ScriptEngine &) {
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
	robotutor::ExecutableParser parser(commands);
	
	try {
		robotutor::parse(parser, std::cin);
		parser.finish();
		command::SharedPtr script = parser.result();
		std::cout << *script;
	} catch (std::exception const & e) {
		std::cerr << "Error during parsing: " << e.what() << "." << std::endl;
	}
}
