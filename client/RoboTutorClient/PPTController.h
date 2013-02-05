#pragma once
class PPTController
{
public:
	PPTController(void);
	~PPTController(void);

	void nextSlide();
	void previousSlide();
	void setSlide(int nr);
	void startPresentation();
	void stopPresentation();
	int getSlideNumber();

private:
	PowerPoint::_ApplicationPtr ppt_;
};

