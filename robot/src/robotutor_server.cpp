#include <iostream>
#include <stdexcept>
#include <functional>

#include <boost/asio/io_service.hpp>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "script_engine.hpp"
#include "noise_detector.hpp"
#include "messages.pb.h"


using namespace robotutor;

void help() {
	std::cout << "Robotutor server v0.7b\n";
	std::cout << "Usage: robotutor-server <options>\n";
	std::cout << "Options:\n";
	std::cout << "-h Print this help message.\n";
	std::cout << "-a <address> The address to bind the server to.\n";
}

int main(int argc, char ** argv) {
	// Get nao host from command line.
	std::string nao_host = "localhost";
	
	if (argc == 1) {
		help();
		return 1;
	}
	int i = 1;
	while (i < argc && (argv[i][0] == '-')) {
		switch (argv[i][1]) {
			case 'h':
			case 'H':
				help();
				return 1;
			case 'a':
			case 'A':
				nao_host = argv[++i];
				break;
		}
		i++;
	}
	
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
	
	boost::shared_ptr<NoiseDetector> noise_detector(NoiseDetector::create(ios, broker, "NoiseDetector"));
	
	// Initialize the script engine.
	ScriptEngine engine(ios, broker);
	
	// Load plugins.
	std::cout << "Loaded " << engine.loadPlugins("lib") << " plugins." << std::endl;
	
	// Register accept handler.
	engine.server.on_accept = [&engine] (SharedServerConnection connection) {
		std::cout << connection->socket().remote_endpoint() << ": Connection accepted." << std::endl;
	};
	
	// Function that deals with a noisy classroom.
	auto onNoise = [&engine] (int level) {
		std::cout << "Noise detected." << std::endl;
	};
	
	// Run the IO service.
	while (true) {
		try {
			ios.run();
			
		} catch (ServerError const & e) {
			std::cout << e.connection->socket().remote_endpoint() << ": " << e.what() << std::endl;
			e.connection->close();
			
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

