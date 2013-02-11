#include <iostream>
#include <boost/make_shared.hpp>

#include "executable.hpp"
#include "script_parser.hpp"


namespace robotutor {
	
	namespace executable {
		
		struct Generic : public Command {
			static Command::SharedPtr create(std::string const & name, ArgList const & arguments) {
				Command::SharedPtr result = boost::make_shared<Generic>();
				result->name      = name;
				result->arguments = arguments;
				return result;
			}
			
			bool step(ScriptEngine &) {
				return true;
			}
		};
		
	}
	

}

using namespace robotutor;

int main() {
	executable::CommandFactory commands;
	commands.add("aap", executable::Generic::create);
	commands.add("mies", executable::Generic::create);
	robotutor::ExecutableParser parser(commands);
	
	try {
		robotutor::parse(parser, std::cin);
		parser.finish();
		std::cout << *parser.result();
	} catch (std::exception const & e) {
		std::cerr << "Error during parsing: " << e.what() << "." << std::endl;
	}
}
