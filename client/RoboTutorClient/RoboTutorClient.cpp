// RoboTutorClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	RCF::RcfInitDeinit rcfInit;

	// Initializes the COM library on the current thread and identifies the 
	// concurrency model as single-thread apartment (STA). 
	// [-or-] CoInitialize(NULL);
	// [-or-] CoCreateInstance(NULL);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);//APARTMENTTHREADED);

	PPTController ppt;

	// Register the client
	RCF::RcfServer callbackServer( RCF::TcpEndpoint(0) );
	callbackServer.bind<PPTInterface>(ppt);
	callbackServer.start();
	RcfClient<PPTInterface> client(( RCF::TcpEndpoint("192.168.1.102", 50001) ));
	RCF::createCallbackConnection(client, callbackServer);

	// Open powerpoint
	ppt.openPresentation("C:\\Users\\hansgaiser\\Desktop\\First slide.pptx");
	ppt.startSlideShow();

	std::cout << "Waiting for server commands, press enter to exit." << std::endl;
	std::cin.get();

	ppt.closePresentation();
	ppt.closePowerpoint();

	// Uninitialize COM for this thread
	CoUninitialize();

	return 0;
}

