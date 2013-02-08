#include <list>
#include <memory>
#include <functional>
#include <atomic>
#include <boost/asio.hpp>

#ifndef ROBOTUTOR_SERVER_HPP_
#define ROBOTUTOR_SERVER_HPP_

namespace robotutor {
	// Forward declarations.
	class Server;
	class Connection;
	
	/// RDP server.
	class Server {
		public:
			typedef std::function<void(Connection const & connection, std::string const & message)> MessageHandler;
			
		protected:
			typedef boost::asio::ip::tcp             tcp;
			typedef tcp::socket                      SocketType;
			typedef tcp::acceptor                    AcceptorType;
			typedef tcp::endpoint                    EndpointType;
			typedef boost::system::error_code        ErrorCode;
			friend class Connection;
			
			/// ASIO IO service.
			boost::asio::io_service & ios_;
			
			/// The acceptor.
			AcceptorType acceptor_;
			
			/// The local endpoint.
			EndpointType endpoint_;
			
			/// List of open connections.
			std::list<Connection> connections_;
			
		public:
			/// Construct a Server using the specified IO service and endpoint.
			Server(boost::asio::io_service & ios, EndpointType const & endpoint);
			
			/// Destruct the server, cleaning up all owned connections in the process.
			~Server();
			
			/// Start listening for incoming connections.
			void start();
			
			/// Get a reference to the ASIO IO service.
			boost::asio::io_service       & ios()       { return ios_; }
			boost::asio::io_service const & ios() const { return ios_; }
			
			/// Send a message to all connected clients.
			/**
			 * \param message The message to send.
			 */
			void sendMessage(std::string const & message);
			
			/// The message handler to invoke when a message is received.
			MessageHandler message_handler;
			
		protected:
			/// Asynchronously accept a new connection.
			void asyncAccept_();
			
			/// Handle an accepted connection.
			/**
			 * \param connection The connection that just became valid.
			 * \param error      Describes the error that occured, if any did.
			 */
			void handleAccept_(Connection & connection, ErrorCode const & error);
	};
	
	/// Connection to a RDP client.
	class Connection {
		protected:
			typedef boost::asio::ip::tcp             tcp;
			typedef tcp::socket                      SocketType;
			typedef boost::system::error_code        ErrorCode;
			typedef boost::asio::streambuf           StreamBuf;
			typedef boost::asio::deadline_timer      Timer;
			friend class Server;
			
			/// The owning server.
			Server & server_;
			
			/// Iterator to ourself in the server's list.
			std::list<Connection>::iterator index_;
			
			/// The socket for communication.
			SocketType socket_;
			
			/// A buffer for reading from the connection.
			StreamBuf read_buffer_;
			
			/// An istream for convenient access to the read buffer.
			std::istream read_stream_;
			
			/// Flag to remember if we already posted our destruction.
			std::atomic_flag death_imminent_;
			
			/// Timer to periodically send an Alive? query.
			Timer heartbeat_timer_;
			
			/// Timer to check if the response arrives in time.
			Timer dead_timer_;
				
		public:
			/// Construct a Connection owned by the given server.
			/**
			 * \param server The server that created us.
			 */
			Connection(Server & server);
			
			/// Deconstruct the connection.
			~Connection();
			
			/// Get a reference to the connection's socket.
			/**
			 * \return The socket.
			 */
			SocketType       & socket()       { return socket_; }
			SocketType const & socket() const { return socket_; }
			
			/// Check if the connection is open.
			/**
			 * \param return True if the connetion is open and can be used.
			 */
			bool isOpen() const { return socket_.is_open(); }
			
			/// Send a message over the connection.
			/**
			 * \param message The message (without trailing newline).
			 */
			void sendMessage(std::string const & message);
			
			/// Asynchronously send a message over the connection.
			/**
			 * \param buffer A shared buffer holding the message (including trailing newline).
			 */
			void asyncWriteMessage(std::shared_ptr<std::string const> buffer);
			
		protected:
			/// Start an asynchronous read operation.
			void asyncReadMessage_();
			
			/// Handle a write operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleWrite_(ErrorCode const & error, std::size_t bytes_transferred, std::shared_ptr<std::string const> buffer);
			
			/// Handle a read operation.
			/**
			 * \param error             Describes the error that occured, if any did.
			 * \param bytes_transferred The amount of bytes transferred for this operation.
			 */
			void handleRead_(ErrorCode const & error, std::size_t bytes_transferred);
			
			/// Start a new heartbeat.
			void startHeartbeatTimer_();
			
			/// Start the dead timer.
			void startDeadTimer_();
			
			/// Handle a heartbeat timer expire.
			/**
			 * \param error Describes the error that occured, if any did.
			 */
			void handleHeartbeat_(ErrorCode const & error);
			
			/// Handle a alive timer expire.
			/**
			 * \param error Describes the error that occured, if any did.
			 */
			void handleDead_(ErrorCode const & error);
			
			/// Destroy the connection in a safe manner.
			/**
			 * This method closes the socket first, and then posts a task
			 * to the ASIO IO service to destroy the connection.
			 * This way, all queued handlers will execute first and can
			 * still safely use the connection object.
			 */
			void destroy_();
			
	};
}
#endif
