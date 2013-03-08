#include "PptController.h"

//DWORD WINAPI AutomatePowerPointByImport(LPVOID lpParam);

void PptController::init() {
	try
	{
		// CreateInstance
		HRESULT hr = ppt_.CreateInstance(__uuidof(PowerPoint::Application));
		if (FAILED(hr))
		{
			wprintf(L"CreateInstance failed w/err 0x%08lx\n", hr);
			return;
		}

		_putws(L"PowerPoint.Application is started");

		// By default PowerPoint is invisible, till you make it visible:
		ppt_->put_Visible(Office::MsoTriState::msoFalse);
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::closePowerpoint()
{
	try
	{
		ppt_->Quit();
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::openPresentation(std::string fileName)
{
	_putws(L"Opening presentation..");
	try
	{
		ppt_->put_Visible(Office::MsoTriState::msoTrue);
		pres_ = ppt_->Presentations->Open(_bstr_t(fileName.c_str()),
										  Office::MsoTriState::msoTrue,
										  Office::MsoTriState::msoTrue,
										  Office::MsoTriState::msoTrue);
	
		_putws(L"A new presentation is opened");
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::closePresentation()
{
	try
	{
		pres_->Close();
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::startSlideShow()
{
	try
	{
		pres_->SlideShowSettings->Run();
		show_ = ppt_->SlideShowWindows->Item(1)->View;
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::setSlide(int nr, bool relative)
{
	try
	{
		int slide = 0;

		if (relative)
			slide = show_->GetSlide()->SlideIndex;

		slide += nr;

		if (slide > pres_->GetSlides()->Count)
			show_->Next();
		else
			show_->GotoSlide(slide, Office::MsoTriState::msoTrue);
	}
	catch (_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
	}
}

void PptController::createSlide(std::string pictureFile)
{
	// pictureFile in this format: http://192.168.0.108/test.jpg
	int curr_slide_nr = show_->Slide->GetSlideNumber();

	PowerPoint::_SlidePtr current_slide = pres_->Slides->Add(curr_slide_nr + 1, PowerPoint::ppLayoutPictureWithCaption);

	current_slide->Shapes->AddPicture(_bstr_t(pictureFile.c_str()),
									  Office::MsoTriState::msoFalse, 
									  Office::MsoTriState::msoTrue, 
									  200, 
									  100, 
									  320, 
									  240);
}