// RoboTutorClient.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include <boost/asio.hpp>

#include "protobuf.hpp"
#include "protocol/messages.pb.h"
#include "connection.hpp"
#include "client.hpp"

#include "PptController.h"

PPTController *ppt = NULL;

int main(int argc, char ** argv)
{
	std::string presentation;
	if (argc < 2)
		presentation = "C:\\Users\\hansgaiser\\Desktop\\Nao.ppt";
	else
		presentation = argv[1];
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

