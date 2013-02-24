#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>


namespace ascf {
	template<typename Protocol, bool IsServer> class Connection;
	
	/// Exception class for connection errors.
	template<typename Protocol, bool IsServer>
	class ConnectionError : public boost::system::system_error {
		protected:
			/// The connection that caused the error.
			Connection<Protocol, IsServer> & connection_;
			
		public:
			/// Construct a connection error.
			/**
			 * \param connection The connection that caused the error.
			 * \param error The underlying boost system error.
			 */
			ConnectionError(Connection<Protocol, IsServer> & connection, boost::system::error_code const & error) :
				boost::system::system_error(error),
				connection_(connection) {}
	};
	
	
	/// Template to extract the send message type from a protocol
	template<typename Protocol, bool IsServer>
	struct WriteMessage {
		typedef typename Protocol::ClientMessage MessageType;
	};
	
	template<typename Protocol>
	struct WriteMessage<Protocol, true> {
		typedef typename Protocol::ServerMessage MessageType;
	};
	
	/// Template to extract the receive message type from a protocol
	template<typename Protocol, bool IsServer>
	struct ReadMessage {
		typedef typename Protocol::ServerMessage MessageType;
	};
	
	template<typename Protocol>
	struct ReadMessage<Protocol, true> {
		typedef typename Protocol::ClientMessage MessageType;
	};
	
	
	/// Connection class.
	template<typename Protocol, bool IsServer>
	class Connection : public std::enable_shared_from_this<Connection<Protocol, IsServer>> {
		public:
			typedef typename WriteMessage<Protocol, IsServer>::MessageType WriteMessage;
			typedef typename ReadMessage <Protocol, IsServer>::MessageType ReadMessage;
			typedef Connection<Protocol, IsServer>      ConnectionType;
			typedef boost::asio::streambuf              StreamBuf;
			typedef boost::system::error_code           ErrorCode;
			typedef ConnectionError<Protocol, IsServer> Error;
			
		protected:
			/// The socket for communication.
			typename Protocol::Socket socket_;
			
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
			explicit Connection(must_be_shared_, typename Protocol::Socket && socket) :
				socket_(std::forward<typename Protocol::Socket>(socket)) {}
			
			/// Deconstruct the connection.
			virtual ~Connection() {}
			
			/// Get a reference to the socket used by the connection.
			/**
			 * \return A const reference to the socket.
			 */
			typename Protocol::Socket const & socket() const { return socket_; }
			
			/// Check if the connection is open.
			/**
			 * \param return True if the connetion is open and can be used.
			 */
			bool isOpen() const { return socket_.is_open(); }
			
			/// Close the connection.
			virtual void close() {
				ErrorCode error;
				socket_.shutdown(Protocol::Socket::shutdown_type::shutdown_both, error);
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
				auto handler    = [connection, buffer](ErrorCode const & error, std::size_t bytes_transferred) {
					connection->handleWrite_(error, bytes_transferred, buffer);
				};
				boost::asio::async_write(socket_, boost::asio::buffer(*buffer), handler);
			}
			
			/// Start an asynchronous read operation.
			void asyncRead_() {
				auto connection = this->shared_from_this();
				auto handler = [connection](ErrorCode const & error, std::size_t bytes_transferred) {
					connection->handleRead_(error, bytes_transferred);
				};
				
				socket_.async_read_some(read_buffer_.prepare(1024), handler);
			}
			
			/// Handle a write operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleWrite_(ErrorCode const & error, std::size_t bytes_transferred, std::shared_ptr<std::string const> buffer) {
				// Destroy the connection if an error occured.
				if (error) throw Error(*this, error);
			}
			
			/// Handle a read operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleRead_(ErrorCode const & error, std::size_t bytes_transferred) {
				// Destroy the connection if an error occured.
				if (error) {
					throw Error(*this, error);
				
				// Otherwise check the received data for a whole message and process it.
				} else {
					read_buffer_.commit(bytes_transferred);
					
					// Consume all available complete messages in the buffer.
					auto handler = [this] (ReadMessage && message) {
						handleMessage_(std::forward<ReadMessage>(message));
					};
					while (Protocol::template consumeMessage<ReadMessage>(read_buffer_, handler));
					
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
