#pragma once

#include <boost/asio.hpp>

#include "../ascf/server.hpp"
#include "../ascf/protobuf.hpp"
#include "../protocol/messages.pb.h"



namespace robotutor {
	class ScriptEngine;
	
	typedef ascf::ProtocolBuffers<ClientMessage, RobotMessage> Protocol;
	
	/// Engine to interact with remote clients.
	class ServerEngine : public ascf::Server<Protocol> {
		protected:
			/// Reference to the parent script engine.
			ScriptEngine & parent_;
			
		public:
			/// Construct the server engine.
			/**
			 * \param parent The parent script engine.
			 * \param ios The IO service to use.
			 */
			ServerEngine(ScriptEngine & parent, boost::asio::io_service & ios) :
				ascf::Server<Protocol>(ios),
				parent_(parent) {}
			
	};
}

