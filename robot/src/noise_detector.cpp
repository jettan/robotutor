#include <boost/asio.hpp>

#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>

#include "noise_detector.hpp"

namespace robotutor {
	
	/// Constructor
	NoiseDetector::NoiseDetector(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALSoundExtractor(broker, name)
	{
		setModuleDescription("RoboTutor Noise Detection Module");
		threshold = 150;
	}
	
	/// Deconstructor
	NoiseDetector::~NoiseDetector() {
		stopDetection();
	}
	
	/// Initializes the audio device and starts detection.
	void NoiseDetector::init() {
		audioDevice->callVoid("setClientPreferences", getName(), 16000, (int)AL::FRONTCHANNEL, 0 );
		startDetection();
	}
	
	/// This function will be automatically called by the module ALAudioDevice
	/// every 170ms with the appropriate audio buffer (front channel at 16000Hz)
	void NoiseDetector::process(const int & nbOfChannels, const int & nbrOfSamplesByChannel, const AL_SOUND_FORMAT * buffer, const AL::ALValue & timeStamp) {
		/// Compute the maximum value of the front microphone signal.
		int maxValueFront = 0;
		for(int i = 0 ; i < nbrOfSamplesByChannel ; i++)
		{
			if(buffer[i] > maxValueFront)
			{
				maxValueFront = buffer[i];
			}
		}
		
		/// Call on_noise function when classroom is too noisy
		//TODO: threshold in config file
		if(maxValueFront >= threshold)
		{
			on_noise(maxValueFront);
		}
	}

}
