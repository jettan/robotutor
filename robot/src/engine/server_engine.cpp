#include "server_engine.hpp"

namespace robotutor {
	
	/// Change the slide of all connected client.
	/**
	 * \param offset The offset.
	 * \param relative If true, the offset is relative to the current slide, otherwise to zero.
	 */
	void ServerEngine::slide(int offset, bool relative) {
		RobotMessage message;
		message.mutable_slide()->set_relative(relative);
		message.mutable_slide()->set_offset(offset);
		sendMessage(message);
	}
	
}
