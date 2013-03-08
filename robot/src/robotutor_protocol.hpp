#pragma once

#include <memory>

#include "ascf/server.hpp"
#include "ascf/client.hpp"
#include "ascf/protobuf.hpp"
#include "messages.pb.h"

namespace robotutor {
	typedef ascf::ProtocolBuffers<ClientMessage, RobotMessage> Protocol;
	
	typedef ascf::Server<Protocol> Server;
	typedef ascf::ServerConnection<Protocol> ServerConnection;
	typedef ascf::Client<Protocol> Client;
	
	typedef std::shared_ptr<Server>           SharedServer;
	typedef std::shared_ptr<ServerConnection> SharedServerConnection;
	typedef std::shared_ptr<Client>           SharedClient;
	
	typedef ascf::ServerError<Protocol>       ServerError;
	typedef ascf::ClientError<Protocol>       ClientError;
}
