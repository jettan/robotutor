#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>

#include <boost/asio.hpp>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "parser/parse.hpp"
#include "parser/command_parser.hpp"
#include "engine/script_engine.hpp"
#include "noise_detector.hpp"

#include "command/speech_commands.hpp"
#include "command/behavior_commands.hpp"
#include "command/presentation_commands.hpp"


using namespace robotutor;

command::Factory factory;

void registerCommands() {
	factory.add<command::Stop>();
	factory.add<command::Execute>();
	factory.add<command::Behavior>();
	factory.add<command::Slide>();
	factory.add<command::ShowImage>();
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

/// Parse the script.
void parse(int argc, char ** argv, ScriptEngine & engine, boost::asio::io_service &ios) {
	// Register all commands.
	registerCommands();
	
	command::SharedPtr script;
	try {
		CommandParser parser(factory);
		// If a command line argument is given, it's an input file to read.
		if (argc > 2) {
			script = parseFile(argv[2]);
		} else  {
			script = parseStream(std::cin);
		}
		
		ios.post([&engine, script] () {
			engine.load(script);
			engine.run();
		});
	} catch (std::exception const & e) {
		std::cerr << "Failed to parse input: " << e.what() << std::endl;
		ios.stop();
	}
}

int main(int argc, char ** argv) {
	// Get nao host from command line.
	std::string nao_host = "localhost";
	if (argc > 1) nao_host = argv[1];
	
	// The main IO service.
	boost::asio::io_service ios;
	
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
	
	// Initialize the script engine.
	ScriptEngine engine(ios, broker);
	
	
	// Stop the io service when the speech engine is done.
	auto onDone = [&ios] () {
		ios.stop();
	};
	
	engine.on_done.connect(onDone);
	
	// Parse input in a different thread.
	std::thread parse_thread([argc, argv, &engine, &ios] () {
		parse(argc, argv, engine, ios);
	});
	
	// Run the IO service and make sure all threads are joined before exiting.
	ios.run();
	parse_thread.join();
	engine.join();
	
	broker->shutdown();
	
	return 0;
}

