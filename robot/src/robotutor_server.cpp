#include <iostream>
#include <fstream>
#include <stdexcept>
#include <functional>

#include <boost/asio/io_service.hpp>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "parse.hpp"
#include "script_parser.hpp"
#include "script_engine.hpp"
#include "noise_detector.hpp"
#include "messages.pb.h"

#include "commands/speech_commands.hpp"
#include "commands/behavior_commands.hpp"
#include "commands/presentation_commands.hpp"



using namespace robotutor;

command::Factory factory;

void registerCommands() {
	factory.add<command::Stop>();
	factory.add<command::Execute>();
	factory.add<command::Behavior>();
	factory.add<command::Slide>();
	factory.add<command::ShowImage>();
}


command::SharedPtr parseFile(command::Factory & factory, std::string const & name) {
	std::ifstream stream(name);
	if (!stream.good()) throw std::runtime_error("Failed to open file `" + name + "'.");
	return parseScript(factory, stream);
}

void handleMessage(ScriptEngine & engine, ClientMessage && message) {
	std::cout << "Message received: " << message.DebugString() << std::endl;
	if (message.has_run_script()) {
		std::shared_ptr<command::Command> script;
		
		// Check if there is a script to parse.
		try {
			if (message.run_script().has_script()) {
				script = parseScript(factory, message.run_script().script());
			} else if (message.run_script().has_file()) {
				script = parseFile(factory, message.run_script().file());
			}
		} catch (std::exception const & e) {
			std::cout << "Error parsing script: " << e.what() << std::endl;
		}
		
		// Run the parsed script.
		if (script) {
			std::cout << "Script parsed.";
			// If the engine is busy, stop it and wait for everything to finish before running the new script.
			if (engine.busy()) {
				auto load_script = [&engine, script] () {
					engine.load(script);
					engine.run();
				};
				engine.stop(load_script);
				
			// If the engine wasn't busy, just run the script.
			} else {
				engine.load(script);
				engine.run();
			}
		}
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
	
	// Register message handler.
	auto on_message = [&engine] (SharedServerConnection connection, ClientMessage && message) {
		handleMessage(engine, std::forward<ClientMessage>(message));
	};
	// Register accept handler.
	auto on_accept = [&engine] (SharedServerConnection connection) {
		std::cout << connection->socket().remote_endpoint() << ": Connection accepted." << std::endl;
	};
	engine.server.on_message = on_message;
	engine.server.on_accept  = on_accept;
	
	// Stop the io service when the speech engine is done.
	engine.on_done.connect(std::bind(&boost::asio::io_service::stop, &ios));
	
	// Run the IO service.
	while (true) {
		try {
			ios.run();
			
		} catch (ServerError const & e) {
			std::cout << e.connection.socket().remote_endpoint() << ": " << e.what() << std::endl;
			e.connection.close();
			
		} catch (std::exception const & e) {
			ios.reset();
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
	
	// Make sure all threads are joined before exiting
	engine.join();
	
	broker->shutdown();
	
	return 0;
}
