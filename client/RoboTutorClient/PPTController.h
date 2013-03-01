#pragma once

#include <string>
#include <algorithm>

//#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

//#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

//#import "libid:91493440-5A91-11CF-8700-00AA0060263B" \
//	rename("RGB", "VisioRGB")
// [-or-]
#import "C:\\Program Files\\Microsoft Office\\Office12\\MSPPT.OLB" \
	rename("RGB", "VisioRGB")

class PPTController
{
public:
	PPTController(void);
	~PPTController(void);
	void openPresentation(std::string fileName);
	void closePresentation();
	void closePowerpoint();
	void startSlideShow();
	void setSlide(int nr, bool relative);

private:
	PowerPoint::_ApplicationPtr ppt_;
	PowerPoint::_PresentationPtr pres_;
	PowerPoint::SlideShowViewPtr show_;
};

