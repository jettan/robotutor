#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <functional>

#include <boost/asio/io_service.hpp>

#include "robotutor_protocol.hpp"


using namespace robotutor;

namespace {
	/// The main IO service.
	boost::asio::io_service ios;
	std::thread read_thread;
	int argc;
	char * * argv;
	int error = 0;
	
	void stop(int error) {
		::error = error;
		ios.stop();
	}
	
	void onMessageSent(SharedClient client, Client::ErrorCode const & error) {
		if (error) {
			std::cout << "Error sending message: " << error.message() << std::endl;
			stop(-3);
		} else {
			std::cout << "Message sent." << std::endl;
			stop(0);
		}
	}
	
	void readScript(SharedClient client) {
		std::stringstream buffer;
		if (argc > 3) {
			std::ifstream file(argv[3]);
			if (!file.good()) {
				std::cout << "Failed to read input file." << std::endl;
				return stop(-2);
			}
			buffer << file.rdbuf();
		} else {
			buffer << std::cin.rdbuf();
		}
		
		std::string script = buffer.str();
		ios.post([script, client] () {
			ClientMessage message;
			message.mutable_run()->set_script(script);
			client->sendMessage(message, onMessageSent);
		});
	}
	
	void onConnect(SharedClient client, Client::ErrorCode const & error) {
		std::string command(argv[2]);
		if (error) {
			std::cout << "Connection error: " << error.message() << std::endl;
			stop(-1);
		} else if (command == "run") {
			read_thread = std::thread([client] () {
				readScript(client);
			});
		} else if (command == "stop") {
			ClientMessage message;
			message.mutable_stop();
			client->sendMessage(message, onMessageSent);
		} else if (command == "pause") {
			ClientMessage message;
			message.mutable_pause();
			client->sendMessage(message, onMessageSent);
		} else if (command == "resume") {
			ClientMessage message;
			message.mutable_resume();
			client->sendMessage(message, onMessageSent);
		}
	}
}

int main(int argc, char ** argv) {
	::argc = argc;
	::argv = argv;
	
	if (argc < 3) {
		std::cout << "Usage: " << std::string(argv[0]) << " server-ip command [options]" << std::endl;
		return -1;
	}
	
	// Get the server IP from command line.
	std::string host = "127.0.0.1";
	std::string command;
	host    = argv[1];
	
	auto client = Client::create(ios);
	client->connectIp4(host, 8311, onConnect);
	
	// Run the IO service.
	try {
		ios.run();
		
	} catch (ClientError const & e) {
		std::cout << "Connection error: " << e.what() << std::endl;
		e.connection->close();
		
	} catch (std::exception const & e) {
		ios.reset();
		std::cout << "Error: " << e.what() << std::endl;
	}
	
	if (read_thread.joinable()) read_thread.join();
	
	return error;
}

