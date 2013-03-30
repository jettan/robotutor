#include <string>
#include <atomic>

#include <boost/asio/system_timer.hpp>
#include <boost/random/mersenne_twister.hpp>


namespace robotutor {
	
	class BehaviorEngine;
	
	class PoseChanger {
		protected:
			/// The IO service to use.
			boost::asio::io_service & ios_;
			
			/// The random generator to use.
			boost::random::mt19937 & random_;
			
			/// The behavior engine to control.
			BehaviorEngine & behavior_engine_;
			
			/// Timer to wait random periods.
			boost::asio::system_timer timer_;
			
			/// True if the engine was started.
			std::atomic_flag started_;
			
		public:
			/// The prefix to run random behaviors from.
			std::string prefix;
			
			/// The minimum time in milliseconds to wait between behaviors.
			unsigned int min;
			
			/// The maximum time in milliseconds to wait between behaviors.
			unsigned int max;
			
		public:
			/// Create a pose changer.
			/**
			 * \param ios The IO service to use.
			 * \param random The random generator to use.
			 * \param behavior_engine The behavior engine to control.
			 * \param prefix The prefix to select behaviors from.
			 * \param min The minimum time in milliseconds between two random poses.
			 * \param max The maximum time in milliseconds between two random poses.
			 */
			PoseChanger(boost::asio::io_service & ios, boost::random::mt19937 & random, BehaviorEngine & behavior_engine, std::string prefix, unsigned int min = 1000, unsigned int max = 4000);
			
			/// Start executing random behaviors.
			void start();
			
			/// Stop executing random behaviors.
			void cancel();
			
		protected:
			/// Wait a random period before calling the timeout function.
			void asyncWaitRandom_();
			
			/// Handle a timeout.
			/**
			 * \param error The error that occured, if any.
			 */
			void handleTimeout_(boost::system::error_code const & error);
	};
	
}
