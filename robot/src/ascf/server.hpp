#pragma once

#include <memory>
#include <functional>
#include <list>
#include <iterator>

#include <boost/asio.hpp>

#include "connection.hpp"

namespace ascf {
	
	template<typename Protocol> class ServerConnection;
	template<typename Protocol> class Server;
	
	template<typename Protocol>
	using ServerConnectionIdentifier = typename std::list<std::shared_ptr<ServerConnection<Protocol>>>::iterator;
	
	/// Specialized connection for servers.
	template<typename ProtocolT>
	class ServerConnection : public Connection<ProtocolT, true> {
		friend class Server<ProtocolT>;
		
		public:
			typedef ProtocolT                    Protocol;
			typedef Connection<Protocol, true>   BaseType;
			
		protected:
			/// The server associated with the connection.
			Server<Protocol> & server_;
			
			/// Protocol defined extension.
			typename Protocol::ServerConnectionExtension extension_;
			
			/// Iterator to the connection in the server's list.
			ServerConnectionIdentifier<Protocol> identifier_;
			
		public:
			/// Construct a server connection.
			/**
			 * \param must_be_shared Token to prevent direct construction.
			 * \param server The server managing us.
			 * \param socket The socket to use.
			 */
			ServerConnection(typename BaseType::must_be_shared_ must_be_shared, Server<Protocol> & server, typename Protocol::Transport::socket && socket) :
				BaseType(must_be_shared, std::forward<typename Protocol::Transport::socket>(socket)),
				server_(server),
				extension_(*this) {}
			
			/// Virtual deconstructor.
			virtual ~ServerConnection() {}
			
			/// Get the protocol defined extension object.
			typename Protocol::ServerConnectionExtension       & extension()       { return extension_; };
			/// Get the protocol defined extension object.
			typename Protocol::ServerConnectionExtension const & extension() const { return extension_; };
			
			/// Get the server associated with the connection.
			Server<Protocol>       & server()       { return server_; };
			/// Get the server associated with the connection.
			Server<Protocol> const & server() const { return server_; };
			
			/// Close the connection.
			void close() {
				BaseType::close();
				extension_.handleClose();
				server_.connections_.erase(identifier_);
			}
			
		protected:
			/// Create a new server connection.
			/**
			 * \param server The server managing us.
			 * \param socket The socket to use.
			 * \return A shared pointer to a new server connection.
			 */
			static std::shared_ptr<ServerConnection<Protocol>> create(Server<Protocol> & server, typename Protocol::Transport::socket && socket) {
				return std::make_shared<ServerConnection<Protocol>>(typename BaseType::must_be_shared_(), server, std::forward<typename Protocol::Transport::socket>(socket));
			}
			
			/// Get a shared pointer to this server connection.
			/**
			 * \return A shared pointer to this server connection.
			 */
			std::shared_ptr<ServerConnection<Protocol>> get_shared_() {
				return std::static_pointer_cast<ServerConnection<Protocol>>(this->shared_from_this());
			}
			
			/// Start reading from the connection.
			void start_() {
				extension_.handleConnect();
				this->asyncRead_();
			}
			
			/// Handle messages by passing them to the user defined message handler.
			/**
			 * \param message The message to handle.
			 */
			void handleMessage_(typename Protocol::ClientMessage && message) {
				if (extension_.handleMessage(message)) {
					server_.handleMessage_(get_shared_(), std::forward<typename Protocol::ClientMessage>(message));
				}
			}
	};
	
	/// ProtoAsio server.
	template<typename ProtocolT>
	class Server {
		friend class ServerConnection<ProtocolT>;
		
		public:
			typedef boost::system::error_code  ErrorCode;
			typedef ProtocolT                  Protocol;
			typedef ServerConnection<Protocol> ConnectionType;
			typedef std::function<void(std::shared_ptr<ServerConnection<Protocol>> connection, typename Protocol::ClientMessage && message)> MessageHandler;
			
			/// The message handler to invoke when a message is received.
			MessageHandler message_handler;
			
		protected:
			/// ASIO IO service.
			boost::asio::io_service & ios_;
			
			/// The acceptor.
			typename Protocol::Transport::acceptor acceptor_;
			
			/// Protocol defined extension.
			typename Protocol::ServerExtension extension_;
			
			/// List of open connections.
			std::list<std::shared_ptr<ServerConnection<Protocol>>> connections_;
			
		public:
			/// Construct a Server using the specified IO service and endpoint.
			/**
			 * \param ios The IO service to use.
			 */
			Server(boost::asio::io_service & ios) :
				ios_(ios),
				acceptor_(ios),
				extension_(*this) {}
			
			/// Get the protocol defined extension object.
			typename Protocol::ServerExtension       & extension()       { return extension_; };
			/// Get the protocol defined extension object.
			typename Protocol::ServerExtension const & extension() const { return extension_; };
			
			/// Get a reference to the ASIO IO service.
			boost::asio::io_service       & ios()       { return ios_; }
			/// Get a reference to the ASIO IO service.
			boost::asio::io_service const & ios() const { return ios_; }
			
			/// Start listening for incoming connections.
			/**
			 * \param endpoint The local endpoint to bind to.
			 */
			void listen(typename Protocol::Transport::endpoint const & endpoint) {
				acceptor_.open(endpoint.protocol());
				acceptor_.set_option(typename Protocol::Transport::acceptor::reuse_address(true));
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
				listen(typename Protocol::Transport::endpoint(arguments...));
			}
			
			/// Listen for incoming TCP/IPv4 connections.
			/**
			 * \param port The port to listen on.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void listenIp4(unsigned short port) {
				listen(boost::asio::ip::tcp::v4(), port);
			}
			
			
			/// Listen for incoming TCP/IPv6 connections.
			/**
			 * \param port The port to listen on.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void listenIp6(unsigned short port) {
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
				std::shared_ptr<typename Protocol::Transport::socket> socket = std::make_shared<typename Protocol::Transport::socket>(ios_);
				auto handler = std::bind(&Server<Protocol>::handleAccept_, this, socket, std::placeholders::_1);
				acceptor_.async_accept(*socket, handler);
			}
			
			/// Handle an accepted connection.
			/**
			 * \param connection The connection that just became valid.
			 * \param error      Describes the error that occured, if any did.
			 */
			void handleAccept_(std::shared_ptr<typename Protocol::Transport::socket> socket, ErrorCode const & error) {
				if (!error) {
					auto connection = ServerConnection<Protocol>::create(*this, std::move(*socket));
					
					if (extension_.handleAccept(connection)) {
						// Register the connection.
						connections_.push_back(connection);
						connection->identifier_ = std::prev(connections_.end());
						
						// Start the asynchronous read loop.
						connection->start_();
					}
				}
				
				// Accept another connection.
				asyncAccept_();
			}
			
			/// Handle messages.
			/**
			 * \param connection The connection that received the message.
			 * \param message    The received message.
			 */
			virtual void handleMessage_(std::shared_ptr<ServerConnection<Protocol>>  connection, typename Protocol::ClientMessage && message) {
				if (message_handler) message_handler(connection, std::forward<typename Protocol::ClientMessage>(message));
			}
	};
	
}
