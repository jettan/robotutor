#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include "behavior_engine.hpp"
#include <stdint.h>

namespace robotutor {

	/// Construct the behaviour engine.
	/**
	 * \param parent The parent script engine.
	 * \param ios The IO service to use.
	 * \param broker The ALBroker to use for communicating with naoqi.
	 */
	BehaviorEngine::BehaviorEngine(ScriptEngine & parent, boost::asio::io_service & ios, boost::shared_ptr<AL::ALBroker> broker) :
		parent_(parent),
		ios_(ios),
		bm_(broker)
		ALSoundExtractor(broker, name) {}
		
	void BehaviorEngine::init() {
	  
	  audioDevice->callVoid("setClientPreferences",
	                        getName(),                //Name of this module
	                        16000,                    //16000 Hz requested
	                        (int)FRONTCHANNEL,        //Front Channels requested
	                        0                         //Deinterleaving is not needed here
	                        );
	
	  startDetection();
	}
		
	BehaviorEngine::~BehaviorEngine() {
		stopDetection();
	}
	
	/// This function will be automatically called by the module ALAudioDevice
	/// every 170ms with the appropriate audio buffer (front channel at 16000Hz)
	void BehaviorEngine::noiseCallback(const int & nbOfChannels,
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
	
	  /// Do stuff when it gets too noisy
	  if(maxValueFront > 20)
	  {
	    // Interrupt speech engine
	    // Change mood
	    // Say something that reflects your mood
	    // Tell speech engine to continue
	  }
	}
	
	/// Run a behaviour asynchronously.
	/**
	 * \param name The name of the behaviour.
	 */
	void BehaviorEngine::run(std::string const & name) {
		if (bm_.isBehaviorInstalled(name)) {
			std::cout << "Running behaviour: `" << name << "'." << std::endl;
			bm_.post.runBehavior(name);
		} else {
			std::cout << "Behavior not found: `" << name << "'." << std::endl;
		}
	}
	
	int BehaviorEngine::rdtsc() {
		asm volatile("rdtsc");
	}
	
	///Draws a random number within the given bounds
	int BehaviorEngine::rnd(int lower, int upper) {
		srand(rdtsc());
		// Of: srand(time(NULL));
			int result = rand() % upper + lower;
			return result;
	}
}

