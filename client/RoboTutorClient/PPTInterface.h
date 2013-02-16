#ifndef PPTINTERFACE_H_
#define PPTINTERFACE_H_

RCF_BEGIN(PPTInterface, "PPTInterface")
	RCF_METHOD_V1(void, openPresentation, std::string);
	RCF_METHOD_V0(void, closePresentation);
	RCF_METHOD_V0(void, closePowerpoint);
	RCF_METHOD_V0(void, startSlideShow);
	RCF_METHOD_V0(void, nextSlide);
	RCF_METHOD_V0(void, previousSlide);
	RCF_METHOD_V1(void, setSlide, int);
RCF_END(MyService);

#endif // ! PPTINTERFACE_H_
