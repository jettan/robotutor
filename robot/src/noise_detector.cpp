#include <boost/asio/io_service.hpp>

#include <alcommon/alproxy.h>

#include "noise_detector.hpp"


namespace robotutor {
	
	/// Construct a noise detector.
	NoiseDetector::NoiseDetector(boost::shared_ptr<AL::ALBroker> broker, std::string const & name) :
		AL::ALSoundExtractor(broker, name)
	{
		setModuleDescription("RoboTutor Noise Detection Module");
		threshold = 150;
	}
	
	/// Create a noise detector.
	/**
	 * \param ios The IO service to use.
	 * \param broker The broker to use for naoqi communication.
	 * \param name The name of the module.
	 */
	boost::shared_ptr<NoiseDetector> NoiseDetector::create(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, std::string const & name) {
		auto result = AL::ALModule::createModule<NoiseDetector>(broker, name);
		result->ios_ = &ios;
		return result;
	}
	
	/// Deconstructor.
	NoiseDetector::~NoiseDetector() {
		stopDetection();
	}
	
	/// Initialize the audio device and start detection.
	void NoiseDetector::init() {
		audioDevice->callVoid("setClientPreferences", getName(), 16000, (int)AL::FRONTCHANNEL, 0 );
		startDetection();
	}
	
	/// Process sound data.
	/**
	 * This function will be automatically called by the module ALAudioDevice
	 * every 170ms with the appropriate audio buffer (front channel at 16000Hz).
	 * 
	 * \param channels The number of channels in the buffer.
	 * \param samples  The number of samples per channel in the buffer.
	 * \param time     The time when the buffer was created.
	 */
	void NoiseDetector::process(const int & channels, const int & samples, const AL_SOUND_FORMAT * buffer, const AL::ALValue & time) {
		// Compute the maximum value of the front microphone signal.
		int max = 0;
		for (int i = 0 ; i < samples; i++) {
			if (buffer[i] > max) max = buffer[i];
		}
		
		std::cout << max << std::endl;
		
		// Emit the signal if the noise level exceeds the threshold.
		if(max >= threshold) ios_->post([this, max] () {
			on_noise(max);
		});
	}
	
}
