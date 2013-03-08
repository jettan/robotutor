#pragma once

#include <string>
#include <memory>
#include <functional>

#include <boost/asio.hpp>


namespace ascf {
	template<typename Protocol, bool IsServer> class Connection;
	template<typename Protocol> class Server;
	template<typename Protocol> class ServerConnection;
	template<typename Protocol> class Client;
	
	/// Default server extension that does nothing.
	template<typename Protocol>
	struct ServerExtension {
		/// The associated server.
		Server<Protocol> & server;
		
		/// Construct the extension.
		/**
		 * \param server The associated server.
		 */
		ServerExtension(Server<Protocol> & server) : server(server) {}
		
		/// Invoked when the server goes into a listening state.
		void handleListening() {}
		
		/// Invoked when the server accepted a connection.
		/**
		 * \return If not true, the server will immideately close the connection.
		 */
		bool handleAccept(std::shared_ptr<ServerConnection<Protocol>> connection) { return true; }
		
		/// Invoked when the server closes the listening socket.
		void handleClose() {}
	};
	
	/// Default connection extension that does nothing.
	template<typename Protocol>
	struct ServerConnectionExtension {
		/// The associated connection.
		ServerConnection<Protocol> & connection;
		
		/// Construct the extension.
		/**
		 * \param connection The associated connection.
		 */
		ServerConnectionExtension(ServerConnection<Protocol> & connection) : connection(connection) {}
		
		/// Invoked when the connection reads a message.
		/**
		 * \param message The message.
		 * \return If not true, the message is not processed further.
		 */
		bool handleMessage(typename Protocol::ClientMessage & message) { return true; }
		
		/// Invoked after the connection was established.
		void handleConnect() {}
		
		/// Invoked after the socket was closed.
		void handleClose() {}
	};
	
	/// Default connection extension that does nothing.
	template<typename Protocol>
	struct ClientExtension {
		/// The associated connection.
		Client<Protocol> & connection;
		
		/// Construct the extension.
		/**
		 * \param connection The associated connection.
		 */
		ClientExtension(Client<Protocol> & connection) : connection(connection) {}
		
		/// Invoked when the connection reads a message.
		/**
		 * \param message The message.
		 * \return If not true, the message is not processed further.
		 */
		bool handleMessage(typename Protocol::ServerMessage & message) { return true; }
		
		/// Invoked after the connection was established.
		void handleConnect() {}
		
		/// Invoked after the socket was closed.
		void handleClose() {}
	};
	
	/// Exception class for connection errors.
	template<typename Protocol>
	class ServerError : public boost::system::system_error {
		public:
			/// The connection that caused the error.
			ServerConnection<Protocol> & connection;
			
			/// Construct a connection error.
			/**
			 * \param connection The connection that caused the error.
			 * \param error The underlying boost system error.
			 */
			ServerError(ServerConnection<Protocol> & connection, boost::system::error_code const & error) :
				boost::system::system_error(error),
				connection(connection) {}
			
			/// Construct a connection error.
			/**
			 * \param connection The connection that caused the error.
			 * \param error The underlying boost system error.
			 */
			ServerError(Connection<Protocol, true> & connection, boost::system::error_code const & error) :
				boost::system::system_error(error),
				connection(static_cast<ServerConnection<Protocol> &>(connection)) {}
	};
	
	/// Exception class for connection errors.
	template<typename Protocol>
	class ClientError : public boost::system::system_error {
		public:
			/// The connection that caused the error.
			Client<Protocol> & connection;
			
			/// Construct a connection error.
			/**
			 * \param connection The connection that caused the error.
			 * \param error The underlying boost system error.
			 */
			ClientError(Client<Protocol> & connection, boost::system::error_code const & error) :
				boost::system::system_error(error),
				connection(connection) {}
			
			/// Construct a connection error.
			/**
			 * \param connection The connection that caused the error.
			 * \param error The underlying boost system error.
			 */
			ClientError(Connection<Protocol, false> & connection, boost::system::error_code const & error) :
				boost::system::system_error(error),
				connection(static_cast<Client<Protocol> &>(connection)) {}
	};
	
	
	/// Template to extract details from a protocol.
	template<typename Protocol, bool IsServer>
	struct ProtocolDetails {
		typedef typename Protocol::ClientMessage WriteMessage;
		typedef typename Protocol::ServerMessage ReadMessage;
		typedef ClientError<Protocol>            Error;
	};
	
	template<typename Protocol>
	struct ProtocolDetails<Protocol, true> {
		typedef typename Protocol::ServerMessage WriteMessage;
		typedef typename Protocol::ClientMessage ReadMessage;
		typedef ServerError<Protocol>            Error;
	};
	
	
	/// Connection class.
	template<typename Protocol, bool IsServer>
	class Connection : public std::enable_shared_from_this<Connection<Protocol, IsServer>> {
		public:
			typedef Connection<Protocol, IsServer>      ConnectionType;
			typedef boost::asio::streambuf              StreamBuf;
			typedef boost::system::error_code           ErrorCode;
			typedef typename ProtocolDetails<Protocol, IsServer>::WriteMessage WriteMessage;
			typedef typename ProtocolDetails<Protocol, IsServer>::ReadMessage  ReadMessage;
			typedef typename ProtocolDetails<Protocol, IsServer>::Error        Error;
			
		protected:
			/// The socket for communication.
			typename Protocol::Transport::socket socket_;
			
			/// Protected struct to prevent direct use of constructor.
			struct must_be_shared_ {};
			
			/// A buffer for reading from the connection.
			StreamBuf read_buffer_;
			
			// Delete copy/move constructor and assignment.
			Connection(Connection const &)                     = delete;
			Connection(Connection &&)                          = delete;
			Connection const & operator = (Connection const &) = delete;
			Connection const & operator = (Connection &&)      = delete;
			
		public:
			/// Construct a connection.
			/**
			 * \param socket The socket to use for the connection.
			 */
			explicit Connection(must_be_shared_, typename Protocol::Transport::socket && socket) :
				socket_(std::forward<typename Protocol::Transport::socket>(socket)) {}
			
			/// Deconstruct the connection.
			virtual ~Connection() {}
			
			/// Get a reference to the socket used by the connection.
			/**
			 * Note that reading from and writing to the socket manually will likely break the message protocol.
			 */
			typename Protocol::Transport::socket       & socket()       { return socket_; }
			/// Get a reference to the socket used by the connection.
			typename Protocol::Transport::socket const & socket() const { return socket_; }
			
			/// Check if the connection is open.
			/**
			 * \param return True if the connetion is open and can be used.
			 */
			bool isOpen() const { return socket_.is_open(); }
			
			/// Close the connection.
			virtual void close() {
				ErrorCode error;
				socket_.shutdown(Protocol::Transport::socket::shutdown_type::shutdown_both, error);
				socket_.close(error);
			}
			
			/// Send a message over the connection.
			/**
			 * \param message The message (without trailing newline).
			 */
			void sendMessage(WriteMessage const & message) {
				asyncWriteBuffer_(Protocol::frameMessage(message));
			}
			
		protected:
			/// Asynchronously send a buffer over the connection.
			/**
			 * \param buffer A shared buffer holding the message size and contents.
			 */
			void asyncWriteBuffer_(std::shared_ptr<std::string const> buffer) {
				// Start the asynchronous write.
				auto connection = this->shared_from_this();
				auto handler    = std::bind(&Connection<Protocol, IsServer>::handleWrite_, this, std::placeholders::_1, std::placeholders::_2, buffer);
				boost::asio::async_write(socket_, boost::asio::buffer(*buffer), handler);
			}
			
			/// Start an asynchronous read operation.
			void asyncRead_() {
				auto connection = this->shared_from_this();
				auto handler    = std::bind(&Connection<Protocol, IsServer>::handleRead_, this, std::placeholders::_1, std::placeholders::_2);
				socket_.async_read_some(read_buffer_.prepare(1024), handler);
			}
			
			/// Handle a write operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleWrite_(ErrorCode const & error, std::size_t bytes_transferred, std::shared_ptr<std::string const> buffer) {
				if (error) throw Error(*this, error);
			}
			
			/// Handle a read operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleRead_(ErrorCode const & error, std::size_t bytes_transferred) {
				if (error) {
					throw Error(*this, error);
					
				// Otherwise check the received data for a whole message and process it.
				} else {
					read_buffer_.commit(bytes_transferred);
					while (auto message = Protocol::template consumeMessage<ReadMessage>(read_buffer_)) {
						handleMessage_(std::move(*message));
					}
					
					// Get more data.
					asyncRead_();
				}
				
			}
			
			/// Process a message.
			/**
			 * \param message The message to handle.
			 */
			virtual void handleMessage_(ReadMessage && message) = 0;
	};
	
}
