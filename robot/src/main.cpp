#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "parser/command_parser.hpp"
#include "parser/parse.hpp"
#include "engine/script_engine.hpp"

#include <unistd.h>


using namespace robotutor;

command::Factory factory;

void registerCommands() {
	factory.add<command::Stop>("stop");
	factory.add<command::Execute>("execute");
}

command::SharedPtr parseStream(std::istream & stream) {
	CommandParser parser(factory);
	parse(parser, stream);
	return parser.result();
}

command::SharedPtr parseFile(std::string const & name) {
	std::ifstream stream(name);
	if (!stream.good()) throw std::runtime_error("Failed to open file `" + name + "'.");
	return parseStream(stream);
}

int main(int argc, char ** argv) {
	
	// Try to parse the script.
	registerCommands();
	
	
	// Try to create a broker.
	boost::shared_ptr<AL::ALBroker> broker;
	try {
		broker = AL::ALBroker::createBroker("robotutor", "0.0.0.0", 54000, "192.168.1.101", 9559);
		AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(broker);
	} catch (...) {
		// Documentation doesn't tell us what to catch.....
		std::cerr << "Failed to connect to robot." << std::endl;
		return -2;
	}
	
	boost::asio::io_service ios;
	
	// Execute the script.
	ScriptEngine engine(broker, ios);
	
	std::thread thread([&ios] () {
		ios.run();
	});

	command::SharedPtr script;
	try {
		CommandParser parser(factory);
		// If a command line argument is given, it's an input file to read.
		if (argc == 2) {
			script = parseFile(argv[1]);
		} else  {
			script = parseStream(std::cin);
		}
	} catch (std::exception const & e) {
		std::cerr << "Failed to parse input: " << e.what() << std::endl;
		return -1;
	}
	
	std::cout << *script << std::endl;
	engine.run(script);
	
	usleep(10 * 1000 * 1000);
	
	ios.stop();
	thread.join();
	return 0;
}

