#include <iostream>

#include <alcommon/albroker.h>

#include "parser/command_parser.hpp"
#include "parser/parse.hpp"


using namespace robotutor;


int main(int argc, char * * argv) {
	
	// Try to parse the script.
	command::SharedPtr script;
	try {
		CommandParser parser;
		parse(parser, std::cin);
		script = parser.result();
	} catch (std::exception const & e) {
		std::cerr << "Failed to parse input: " << e.what() << std::endl;
		return -1;
	}
	
	std::cout << script << std::endl;
	return 0;
	
	// Try to create a broker.
	boost::shared_ptr<AL::ALBroker> broker;
	try {
		broker = AL::ALBroker::create("robotutor", "0.0.0.0", 54000, "192.168.1.2", 9292);
	} catch (...) {
		// Documentation doesn't tell us what to catch.....
		std::cerr << "Failed to connect to robot." << std::endl;
		return -2;
	}
	
	ScriptEngine engine(broker);
	
	return 0;
}
