#pragma once

#include <string>
#include <deque>
#include <functional>
#include <memory>
#include <thread>

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

	class NoiseDetector : public AL::ALSoundExtractor {
		public:
			
			int threshold;
			
			NoiseDetector(boost::shared_ptr<AL::ALBroker> broker, const std::string & name);

			virtual ~NoiseDetector();
	
			/// Signal indicating that the noise level is too high.
			boost::signal<void (int noiseLevel)> on_noise;

			// Initialize the sound module
			void init();

			void process(
				const int & nbOfChannels,
				const int & nbrOfSamplesByChannel,
				const AL_SOUND_FORMAT * buffer,
				const AL::ALValue & timeStamp
			);
	
	};

}
