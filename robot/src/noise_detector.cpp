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
	}
	
	/// Deconstructor
	NoiseDetector::~NoiseDetector() {
		stopDetection();
	}
	
	/// Initializes the audio device and starts detection.
	void NoiseDetector::init()
	{
	  audioDevice->callVoid("setClientPreferences",
		                getName(),                //Name of this module
		                16000,                    //16000 Hz requested
		                (int)FRONTCHANNEL,        //Front Channels requested
		                0                         //Deinterleaving is not needed here
		                );

	  startDetection();
	}

	/// This function will be automatically called by the module ALAudioDevice
	/// every 170ms with the appropriate audio buffer (front channel at 16000Hz)
	void NoiseDetector::noiseCallback(const int & nbOfChannels,
		                   	  const int & nbrOfSamplesByChannel,
		                          const AL_SOUND_FORMAT * buffer,
		                          const ALValue & timeStamp)
	{
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
	  if(maxValueFront > 20)
	  {
	    on_noise(*maxValueFront);
	  }
	}
	
}
