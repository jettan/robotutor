// RoboTutorClient.cpp : Defines the entry point for the console application.
//
// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	
	// Initializes the COM library on the current thread and identifies the 
	// concurrency model as single-thread apartment (STA). 
	// [-or-] CoInitialize(NULL);
	// [-or-] CoCreateInstance(NULL);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	PPTController ppt;
	Client clt;
	
	ppt.openPresentation("C:\\Users\\Anass\\Documents\\First slide.pptx");
	ppt.startSlideShow();
	clt.connectToNao();
	clt.sendToNao();
	while (true)
	{
		if (clt.rcvFromNao() == 0)
			break;
		ppt.nextSlide();
	}
	
	ppt.closePresentation();
	ppt.closePowerpoint();

	clt.closeConnection();

	// Uninitialize COM for this thread
	CoUninitialize();

	return 0;
}

