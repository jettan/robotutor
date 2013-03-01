// RoboTutorClient.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include <boost/asio.hpp>

#include "protobuf.hpp"
#include "protocol/messages.pb.h"
#include "connection.hpp"
#include "client.hpp"

#include "PPTController.h"

typedef ascf::ProtocolBuffers<robotutor::ClientMessage, robotutor::RobotMessage> Protocol;

PPTController *ppt = NULL;

void handleServerMessage(std::shared_ptr<ascf::Client<Protocol>> connection, robotutor::RobotMessage const & message) {

	if (message.has_slide() && ppt != NULL) {
		ppt->setSlide(message.slide().offset(), message.slide().relative());
	}
}

int main(int argc, char ** argv)
{
	std::string presentation;
	if (argc < 2)
		presentation = "C:\\Users\\hansgaiser\\Desktop\\Nao.ppt";
	else
		presentation = argv[1];

	// Initializes the COM library on the current thread and identifies the 
	// concurrency model as single-thread apartment (STA). 
	// [-or-] CoInitialize(NULL);
	// [-or-] CoCreateInstance(NULL);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);//APARTMENTTHREADED);

	ppt = new PPTController();

	// Open powerpoint
	ppt->openPresentation(presentation);
	ppt->startSlideShow();

	boost::asio::io_service ios;
	auto client = ascf::Client<Protocol>::create(ios);
	client->message_handler  = handleServerMessage;
	client->connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("192.168.0.108"), 8311));
	try {
	ios.run();
	} catch (std::exception const &e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	//ppt->closePresentation();
	//ppt->closePowerpoint();

	// Uninitialize COM for this thread
	CoUninitialize();

	return 0;
}

