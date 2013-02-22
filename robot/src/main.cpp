#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "parser/parse.hpp"
#include "parser/command_parser.hpp"
#include "engine/script_engine.hpp"

#include "command/speech_commands.hpp"
#include "command/behavior_commands.hpp"

#include <unistd.h>


using namespace robotutor;

command::Factory factory;

void registerCommands() {
	factory.add<command::Stop>("stop");
	factory.add<command::Execute>("execute");
	factory.add<command::Behavior>("behavior");
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
	// Get nao host from command line.
	std::string nao_host = "localhost";
	if (argc > 1) nao_host = argv[1];
	
	// Register all commands.
	registerCommands();
	
	// Try to create a broker.
	boost::shared_ptr<AL::ALBroker> broker;
	try {
		broker = AL::ALBroker::createBroker("robotutor", "0.0.0.0", 54000, nao_host, 9559);
		AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(broker);
	} catch (...) {
		// Documentation doesn't tell us what to catch.....
		std::cerr << "Failed to connect to robot." << std::endl;
		return -2;
	}
	
	boost::asio::io_service ios;
	
	// Run the io service.
	auto iosRun = [&ios] () {
		ios.run();
	};
	
	// Stop the io service when the speech engine is done.
	auto onDone = [&ios] (SpeechEngine &) {
		ios.stop();
	};
	
	// Initialize the script engine.
	ScriptEngine engine(ios, broker);
	engine.speech->on_done.connect(onDone);
	std::thread thread(iosRun);
	
	command::SharedPtr script;
	try {
		CommandParser parser(factory);
		// If a command line argument is given, it's an input file to read.
		if (argc > 2) {
			script = parseFile(argv[2]);
		} else  {
			script = parseStream(std::cin);
		}
	} catch (std::exception const & e) {
		std::cerr << "Failed to parse input: " << e.what() << std::endl;
		return -1;
	}
	std::cout << *script;
	
	engine.run(script);
	thread.join();
	
	usleep(1000 * 1000);
	
	return 0;
}

