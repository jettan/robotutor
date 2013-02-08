// RoboTutorClient.cpp : Defines the entry point for the console application.
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
	ppt.openPresentation("C:\\Users\\Anass\\Documents\\First slide.pptx");
	ppt.startSlideShow();

	Sleep(5000);
	ppt.nextSlide();
	Sleep(2000);
	ppt.nextSlide();
	ppt.nextSlide();
	ppt.nextSlide();
	Sleep(5000);
	
	ppt.closePresentation();
	Sleep(2000);
	ppt.closePowerpoint();

	// Uninitialize COM for this thread
	CoUninitialize();

	return 0;
}

