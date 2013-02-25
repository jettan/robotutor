#include "server_engine.hpp"

namespace robotutor {
	
	/// Jump to a slide relative from the current slide.
	/**
	 * \param offset The offset.
	 */
	void ServerEngine::adjustSlide(int offset) {
		RobotMessage message;
		message.mutable_slide()->set_relative(true);
		message.mutable_slide()->set_offset(offset);
		sendMessage(message);
	}
	
	/// Jump to a slide.
	/**
	 * \param index The absolute slide number.
	 */
	void ServerEngine::goSlide(unsigned int index) {
		RobotMessage message;
		message.mutable_slide()->set_relative(false);
		message.mutable_slide()->set_offset(index);
		sendMessage(message);
	}
	
}
