#include "stdafx.h"

DWORD WINAPI AutomatePowerPointByImport(LPVOID lpParam);

PPTController::PPTController(void)
{
	// Initializes the COM library on the current thread and identifies the 
	// concurrency model as single-thread apartment (STA). 
	// [-or-] CoInitialize(NULL);
	// [-or-] CoCreateInstance(NULL);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	try
	{
		// Option 2) Create the object using the smart pointer's function,
		// CreateInstance
		HRESULT hr = ppt_.CreateInstance(__uuidof(PowerPoint::Application));
		if (FAILED(hr))
		{
			wprintf(L"CreateInstance failed w/err 0x%08lx\n", hr);
			return;
		}

		_putws(L"PowerPoint.Application is started");


		/////////////////////////////////////////////////////////////////////
		// Make PowerPoint invisible. (i.e. Application.Visible = 0)
		// 

		// By default PowerPoint is invisible, till you make it visible:
		ppt_->put_Visible(Office::MsoTriState::msoTrue);
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}

	// Uninitialize COM for this thread
	CoUninitialize();
}

PPTController::~PPTController(void)
{
}

//
//   FUNCTION: AutomatePowerPointByImport(LPVOID)
//
//   PURPOSE: Automate Microsoft PowerPoint using the #import directive and 
//      smart pointers.
// 
DWORD WINAPI AutomatePowerPointByImport(LPVOID lpParam)
{
	// Initializes the COM library on the current thread and identifies the 
	// concurrency model as single-thread apartment (STA). 
	// [-or-] CoInitialize(NULL);
	// [-or-] CoCreateInstance(NULL);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	try
	{

		/////////////////////////////////////////////////////////////////////
		// Create the PowerPoint.Application COM object using the #import 
		// directive and smart pointers.
		// 

		// Option 1) Create the object using the smart pointer's constructor
		// 
		// _ApplicationPtr is the original interface name, _Application, with a 
		// "Ptr" suffix.
		//PowerPoint::_ApplicationPtr spPpApp(
		//	__uuidof(PowerPoint::Application)	// CLSID of the component
		//	);

		// Option 2) Create the object using the smart pointer's function,
		// CreateInstance
		PowerPoint::_ApplicationPtr spPpApp;
		HRESULT hr = spPpApp.CreateInstance(__uuidof(PowerPoint::Application));
		if (FAILED(hr))
		{
			wprintf(L"CreateInstance failed w/err 0x%08lx\n", hr);
			return 1;
		}

		_putws(L"PowerPoint.Application is started");


		/////////////////////////////////////////////////////////////////////
		// Make PowerPoint invisible. (i.e. Application.Visible = 0)
		// 

		// By default PowerPoint is invisible, till you make it visible:
		spPpApp->put_Visible(Office::MsoTriState::msoTrue);

		/////////////////////////////////////////////////////////////////////
		// Create a new Presentation. (i.e. Application.Presentations.Add)
		// 

		PowerPoint::PresentationsPtr spPres = spPpApp->Presentations;
		//PowerPoint::_PresentationPtr spPre = spPres->Add(Office::msoTrue);
		PowerPoint::_PresentationPtr spPre = spPres->Open(_bstr_t("C:\\Users\\Anass\\Downloads\\C++_app_automates_PowerPoint_(CppAutomatePowerPoint)\\C++\\Debug\\Sample1.pptx"),
														  Office::MsoTriState::msoTrue,
														  Office::MsoTriState::msoTrue,
														  Office::MsoTriState::msoTrue);
		_putws(L"A new presentation is opened");

		//start slideshow
		spPre->SlideShowSettings->Run();
		Sleep(3000);
		spPpApp->SlideShowWindows->Item(1)->View->Next();
			//ActivePresentation.SlideShowSettings.Run();
		/////////////////////////////////////////////////////////////////////
		// Insert a new Slide and add some text to it.
		// 

		/*PowerPoint::SlidesPtr spSlides = spPre->Slides;

		wprintf(L"The presentation currently has %ld slides\n", 
			spSlides->Count);*/



        // Insert a new slide
		/*_putws(L"Insert a slide");
		PowerPoint::_SlidePtr spSlide = spSlides->Add(1, 
			PowerPoint::ppLayoutText);*/

		// Add some texts to the slide
        /*_putws(L"Add some texts");
		PowerPoint::ShapesPtr spShapes = spSlide->Shapes;
		PowerPoint::ShapePtr spShape = spShapes->Item((long)1);
		PowerPoint::TextFramePtr spTxtFrame = spShape->TextFrame;
		PowerPoint::TextRangePtr spTxtRange = spTxtFrame->TextRange;
        spTxtRange->Text = _bstr_t(L"All-In-One Code Framework");*/


		/////////////////////////////////////////////////////////////////////
		// Save the presentation as a pptx file and close it.
		// 
		Sleep(20000);
        _putws(L"Save and close the presentation");

		// Make the file name

		// Get the directory of the current exe.
		/*wchar_t szFileName[MAX_PATH];
		if (!GetModuleDirectory(szFileName, ARRAYSIZE(szFileName)))
		{
			_putws(L"GetModuleDirectory failed");
			return 1;
		}

		// Concat "Sample1.pptx" to the directory
		wcsncat_s(szFileName, ARRAYSIZE(szFileName), L"Sample1.pptx", 12);

		spPre->SaveAs(_bstr_t(szFileName), 
			PowerPoint::ppSaveAsOpenXMLPresentation, Office::msoTriStateMixed);*/

        spPre->Close();


		/////////////////////////////////////////////////////////////////////
		// Quit the PowerPoint application.
		// 

		_putws(L"Quit the PowerPoint application");
		spPpApp->Quit();


		/////////////////////////////////////////////////////////////////////
		// Release the COM objects.
		// 

		// Releasing the references is not necessary for the smart pointers
		// ...
		// spPowerPointApp.Release();
		// ...

	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}

	// Uninitialize COM for this thread
	CoUninitialize();

	return 0;
}