#pragma once
class PPTController
{
public:
	PPTController(void);
	~PPTController(void);
	void openPresentation(std::string fileName);
	void closePresentation();
	void closePowerpoint();
	void startSlideShow();
	void nextSlide();
	void previousSlide();
	void setSlide(int nr);

private:
	PowerPoint::_ApplicationPtr ppt_;
	PowerPoint::_PresentationPtr pres_;
	PowerPoint::SlideShowViewPtr show_;
};

