#pragma once

class PptController
{
public:
	void init();
	void openPresentation(std::string fileName);
	void closePresentation();
	void closePowerpoint();
	void startSlideShow();
	void setSlide(int nr, bool relative);
	void createSlide(std::string pictureFile);

private:
	PowerPoint::_ApplicationPtr ppt_;
	PowerPoint::_PresentationPtr pres_;
	PowerPoint::SlideShowViewPtr show_;
};

