#include "StdAfx.h"

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

		ppt_->put_Visible(Office::MsoTriState::msoTrue);
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

bool PptController::getPresentation(PowerPoint::_PresentationPtr & pres)
{
	try
	{
		if (ppt_->Presentations->GetCount() > 0)
			pres = ppt_->Presentations->Item(_variant_t(long(1)));
		else
			return false;
	}
	catch(_com_error &err)
	{
		wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
		wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
		return false;
	}
	return true;
}

void PptController::openPresentation(std::string fileName)
{
	PowerPoint::_PresentationPtr pres = NULL;
	if (getPresentation(pres))
	{
		_putws(L"Opening presentation..");
		try
		{
			{
				pres = ppt_->Presentations->Open(_bstr_t(fileName.c_str()),
												  Office::MsoTriState::msoTrue,
												  Office::MsoTriState::msoTrue,
												  Office::MsoTriState::msoTrue);
	
				_putws(L"A new presentation is opened");
			}
		}
		catch (_com_error &err)
		{
			wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
			wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
		}
	}
}

void PptController::closePresentation()
{
	PowerPoint::_PresentationPtr pres = NULL;
	if (getPresentation(pres))
	{
		try
		{
			pres->Close();
		}
		catch (_com_error &err)
		{
			wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
			wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
		}
	}
}

void PptController::startSlideShow()
{
	PowerPoint::_PresentationPtr pres = NULL;
	if (getPresentation(pres))
	{
		try
		{
			pres->SlideShowSettings->Run();
			show_ = ppt_->SlideShowWindows->Item(1)->View;
		}
		catch (_com_error &err)
		{
			wprintf(L"PowerPoint throws the error: %s\n", err.ErrorMessage());
			wprintf(L"Description: %s\n", (LPCWSTR) err.Description());
		}
	}
}

void PptController::setSlide(int nr, bool relative)
{
	PowerPoint::_PresentationPtr pres = NULL;
	if (getPresentation(pres))
	{
		try
		{
			int slide = 0;

			if (relative)
				slide = show_->GetSlide()->SlideIndex;

			slide += nr;

			if (slide > pres->GetSlides()->Count)
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
}

void PptController::createSlide(std::string pictureFile)
{
	PowerPoint::_PresentationPtr pres = NULL;
	if (getPresentation(pres))
	{
		// pictureFile in this format: http://192.168.0.108/test.jpg
		int curr_slide_nr = show_->Slide->GetSlideNumber();

		PowerPoint::_SlidePtr current_slide = pres->Slides->Add(curr_slide_nr + 1, PowerPoint::ppLayoutPictureWithCaption);

		current_slide->Shapes->AddPicture(_bstr_t(pictureFile.c_str()),
										  Office::MsoTriState::msoFalse, 
										  Office::MsoTriState::msoTrue, 
										  200, 
										  100, 
										  320, 
										  240);
	}
}
