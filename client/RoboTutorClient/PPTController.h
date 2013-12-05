#pragma once
namespace robotutor{
class RoboTutorClient;
}

class PptController{ 

public:

	void init();
	int monitor();
	void openPresentation(std::string fileName);
	void closePresentation();
	void closePowerpoint();
	void startSlideShow();
	void setSlide(int nr, bool relative);
	void createSlide(std::string pictureFile);
	void createBar();
	void colorBar(char color);
	void resizeBar(char scale);

private:
	PowerPoint::_ApplicationPtr ppt_;
	//PowerPoint::_PresentationPtr pres_;
	PowerPoint::SlideShowViewPtr show_;
	PowerPoint::ShapePtr soundBar;


private:
	bool getPresentation(PowerPoint::_PresentationPtr & pres);
	bool init_done;

};
