#pragma once

#include <memory>
#include <functional>
#include <list>
#include <iterator>

#include <boost/asio.hpp>

#include "connection.hpp"

namespace ascf {
	
	template<typename Protocol> class Server;
	template<typename Protocol> class ServerConnection;
	
	
	/// Specialized connection for servers.
	template<typename Protocol>
	class ServerConnection : public Connection<Protocol, true> {
		friend class Server<Protocol>;
		public:
			typedef Server<Protocol>                                ServerType;
			typedef ServerConnection<Protocol>                      ConnectionType;
			typedef Connection<Protocol, true>                      ParentType;
			typedef typename ServerType::IdentifierType             IdentifierType;
		
		protected:
			/// Reference to the parent server.
			ServerType & server_;
			
			/// Iterator to the connection in the server's list.
			typename ServerType::IdentifierType identifier_;
			
		public:
			/// Construct a server connection.
			/**
			 * \param must_be_shared Token to prevent direct construction.
			 * \param server The server managing us.
			 * \param socket The socket to use.
			 */
			ServerConnection(typename ParentType::must_be_shared_ must_be_shared, ServerType & server, typename Protocol::Socket && socket) :
				ParentType(must_be_shared, std::forward<typename Protocol::Socket>(socket)),
				server_(server) {}
			
			/// Virtual deconstructor.
			virtual ~ServerConnection() {}
			
			/// Create a new server connection.
			/**
			 * \param server The server managing us.
			 * \param socket The socket to use.
			 * \return A shared pointer to a new server connection.
			 */
			static std::shared_ptr<ConnectionType> create(ServerType & server, typename Protocol::Socket && socket) {
				return std::make_shared<ConnectionType>(
					typename ParentType::must_be_shared_(),
					server,
					std::forward<typename Protocol::Socket>(socket)
				);
			}
			
			/// Close the connection.
			virtual void close() {
				ParentType::close();
				server_.unregisterConnection_(identifier_);
			}
			
		protected:
			/// Get a shared pointer to this server connection.
			/**
			 * \return A shared pointer to this server connection.
			 */
			std::shared_ptr<ConnectionType> get_shared_() {
				return std::static_pointer_cast<ConnectionType>(this->shared_from_this());
			}
			
			/// Start reading from the connection.
			void start_() {
				this->asyncRead_();
			}
			
			/// Handle messages by passing them to the user defined message handler.
			/**
			 * \param message The message to handle.
			 */
			virtual void handleMessage_(typename Protocol::ClientMessage && message) {
				server_.handleMessage_(get_shared_(), std::forward<typename Protocol::ClientMessage>(message));
			}
	};
	
	/// ProtoAsio server.
	template<typename Protocol>
	class Server {
		friend class ServerConnection<Protocol>;
		
		protected:
			typedef boost::system::error_code ErrorCode;
			typedef ServerConnection<Protocol> ConnectionType;
			typedef typename std::list<std::shared_ptr<ConnectionType>>::iterator IdentifierType;
			
		public:
			typedef std::function<void(std::shared_ptr<ConnectionType> connection, typename Protocol::ClientMessage && message)> MessageHandler;
			
			/// The message handler to invoke when a message is received.
			MessageHandler message_handler;
			
		protected:
			/// ASIO IO service.
			boost::asio::io_service & ios_;
			
			/// The acceptor.
			typename Protocol::Acceptor acceptor_;
			
			/// List of open connections.
			std::list<std::shared_ptr<ConnectionType>> connections_;
			
		public:
			/// Construct a Server using the specified IO service and endpoint.
			/**
			 * \param ios The IO service to use.
			 */
			Server(boost::asio::io_service & ios) :
				ios_(ios),
				acceptor_(ios) {}
			
			/// Start listening for incoming connections.
			/**
			 * \param endpoint The local endpoint to bind to.
			 */
			void listen(typename Protocol::Endpoint const & endpoint) {
				acceptor_.open(endpoint.protocol());
				acceptor_.set_option(typename Protocol::Acceptor::reuse_address(true));
				acceptor_.bind(endpoint);
				acceptor_.listen();
				asyncAccept_();
			}
			
			/// Start listening for incoming connections.
			/**
			 * \param arguments The arguments for the endpoint constructor.
			 */
			template<typename... Arguments>
			void listen(Arguments... arguments) {
				listen(typename Protocol::Endpoint(arguments...));
			}
			
			/// Listen for incoming TCP/IPv4 connections.
			/**
			 * \param port The port to listen on.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void listen_ip4(unsigned short port) {
				listen(boost::asio::ip::tcp::v4(), port);
			}
			
			
			/// Listen for incoming TCP/IPv6 connections.
			/**
			 * \param port The port to listen on.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void listen6(unsigned short port) {
				listen(boost::asio::ip::tcp::v6(), port);
			}
			
			/// Close the server.
			/**
			 * Stop listening to connections and clear all registered connections.
			 */
			void close() {
				acceptor_.close();
				connections_.clear();
			}
			
			/// Get a reference to the ASIO IO service.
			boost::asio::io_service       & ios()       { return ios_; }
			boost::asio::io_service const & ios() const { return ios_; }
			
			/// Send a message to all connected clients.
			/**
			 * \param message The message to send.
			 */
			void sendMessage(typename Protocol::ServerMessage const & message) {
				auto buffer = Protocol::frameMessage(message);
				
				for (auto connection : connections_) {
					if (connection->isOpen()) {
						connection->asyncWriteBuffer_(buffer);
					}
				}
			}
			
		protected:
			/// Asynchronously accept a new connection.
			void asyncAccept_() {
				auto socket = std::make_shared<typename Protocol::Socket>(ios_);
				
				// Asynchronously accept a connection.
				auto handler = [this, socket] (ErrorCode const & error) {
					handleAccept_(socket, error);
				};
				acceptor_.async_accept(*socket, handler);
			}
			
			/// Unregister a connection from the server.
			/**
			 * \param identifier The identifier returned by registerConnection().
			 */
			void unregisterConnection_(IdentifierType const & identifier) {
				connections_.erase(identifier);
			}
			
			/// Handle an accepted connection.
			/**
			 * \param connection The connection that just became valid.
			 * \param error      Describes the error that occured, if any did.
			 */
			void handleAccept_(std::shared_ptr<typename Protocol::Socket> socket, ErrorCode const & error) {
				if (!error) {
					auto connection = ConnectionType::create(*this, std::move(*socket));
					connections_.push_back(connection);
					connection->identifier_ = std::prev(connections_.end());
					connection->start_();
				}
				
				// Accept another connection.
				asyncAccept_();
			}
			
			/// Handle messages.
			/**
			 * \param connection The connection that received the message.
			 * \param message    The received message.
			 */
			void handleMessage_(std::shared_ptr<ConnectionType> connection, typename Protocol::ClientMessage && message) {
				if (message_handler) message_handler(connection, std::forward<typename Protocol::ClientMessage>(message));
			}
	};
	
}
