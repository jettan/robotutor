#pragma once

#include <string>

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include <alcommon/almodule.h>
#include <alaudio/alsoundextractor.h>


namespace boost {
	namespace asio {
		class io_service;
	}
}

namespace AL {
	class ALBroker;
	class ALSoundExtractor;
}

namespace robotutor {
	
	/// The noise detector monitors the recorded sound level.
	/**
	 * When the noise level exceeds a given threshold, a signal is emitted.
	 */
	class NoiseDetector : public AL::ALSoundExtractor {
		public:
			/// The threshold, in DB?
			int threshold;
			
			/// Signal emitted when the noise level exceeds the threshold.
			boost::signal<void (int noiseLevel)> on_noise;
			
		protected:
			/// The IO service to use.
			boost::asio::io_service * ios_;
			
		public:
			/// Construct a noise detector.
			/**
			 * \param broker The broker to use for naoqi communication.
			 * \param name The name of the module.
			 */
			NoiseDetector(boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Create a noise detector.
			/**
			 * \param ios The IO service to use.
			 * \param broker The broker to use for naoqi communication.
			 * \param name The name of the module.
			 */
			static boost::shared_ptr<NoiseDetector> create(boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker, std::string const & name);
			
			/// Deconstructor.
			~NoiseDetector();
			
			/// Initialize the audio device and start detection.
			void init();
			
			/// Process sound data.
			/**
			 * This function will be automatically called by the module ALAudioDevice
			 * every 170ms with the appropriate audio buffer (front channel at 16000Hz).
			 * \param channels The number of channels in the buffer.
			 * \param samples  The number of samples per channel in the buffer.
			 * \param time     The time when the buffer was created.
			 */
			void process(const int & channels, const int & samples, const AL_SOUND_FORMAT * buffer, const AL::ALValue & time);
	};

}
