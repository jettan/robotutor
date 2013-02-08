#include <boost/date_time.hpp>
#include "server.hpp"

namespace robotutor {
	namespace {
		/// Time between heartbeats.
		auto heartbeat_timeout = boost::posix_time::minutes(1);
		/// Time for a client to respond to a heartbeat before it's considered dead.
		auto dead_timeout      = boost::posix_time::seconds(10);
	}
	
	/// Construct a Server using the specified IO service and endpoint.
	Server::Server(boost::asio::io_service & ios, EndpointType const & endpoint) : ios_(ios), acceptor_(ios), endpoint_(endpoint) {
		// Nothing to do :)
	}


	/// Destruct the server, cleaning up all owned connections in the process.
	Server::~Server() {}


	/// Start listening for incoming connections.
	void Server::start() {
		acceptor_.open(endpoint_.protocol());
		acceptor_.set_option(AcceptorType::reuse_address(true));
		acceptor_.bind(endpoint_);
		acceptor_.listen();
		asyncAccept_();
	}


	/// Send a message to all connected clients.
	void Server::sendMessage(std::string const & message) {
		auto buffer = std::make_shared<std::string const>(message + "\n"); // <--- Change this to non-new line terminated messages!
		for (auto & connection : connections_) {
			if (connection.isOpen()) {
				connection.asyncWriteMessage(buffer);
			}
		}
		
	}

	/// Asynchronously accept a new connection.
	void Server::asyncAccept_() {
		// Create a new connection in the list.
		auto index = connections_.emplace(connections_.end(), *this);
		Connection & connection = connections_.back();
		connection.index_ = index;
		
		// Asynchronously accept a connection.
		auto handler = [this, &connection](ErrorCode const & error) {
			handleAccept_(connection, error);
		};
		acceptor_.async_accept(connection.socket(), handler);
	}


	/// Handle an accepted connection.
	void Server::handleAccept_(Connection & connection, ErrorCode const & error) {
		// Destroy the connection if an error occured.
		if (error) {
			connection.destroy_();
		// Otherwise start reading messages.
		} else {
			connection.startHeartbeatTimer_();
			connection.asyncReadMessage_();
		}
		
		// Accept another connection.
		asyncAccept_();
	}



	/// Construct a Connection owned by the given server.
	Connection::Connection(Server & server)
		: server_(server)
		, socket_(server_.ios())
		, read_stream_(&read_buffer_)
		, heartbeat_timer_(server_.ios())
		, dead_timer_(server_.ios()) {
			death_imminent_.clear();
	}


	/// Deconstruct the connection.
	Connection::~Connection() {
		// Nothing more to do :)
	}


	/// Send a message over the connection.
	/**
	 * \param message The message (without trailing newline).
	 */
	void Connection::sendMessage(std::string const & message) {
		auto buffer = std::make_shared<std::string const>(message + "\n");
		asyncWriteMessage(buffer);
	}


	/// Asynchronously write a message to the connection.
	/**
	 * \param buffer A shared buffer holding the message (including trailing newline).
	 */
	void Connection::asyncWriteMessage(std::shared_ptr<std::string const> buffer) {
		auto handler = [buffer, this](ErrorCode const & error, std::size_t bytes_transferred) {
			handleWrite_(error, bytes_transferred, buffer);
		};
		boost::asio::async_write(socket_, boost::asio::buffer(*buffer), handler);
	}


	/// Start an asynchronous read operation.
	void Connection::asyncReadMessage_() {
		auto handler = [this](ErrorCode const & error, std::size_t bytes_transferred) {
			handleRead_(error, bytes_transferred);
		};
		
		boost::asio::async_read_until(socket_, read_buffer_, '\n', handler);
	}


	/// Handle a completed send operation.
	void Connection::handleWrite_(ErrorCode const & error, std::size_t bytes_transferred, std::shared_ptr<std::string const> buffer) {
		// Release the buffer.
		// This would happen automatically thanks to RAII,
		// but let's make it explicit.
		buffer.reset();
		
		// Destroy the connection if an error occured.
		if (error) {
			destroy_();
		}
	}


	/// Handle a read operation.
	void Connection::handleRead_(ErrorCode const & error, std::size_t bytes_transferred) {
		// Destroy the connection if an error occured.
		if (error) {
			destroy_();
		// Otherwise handle the message and start a new read.
		} else {
			// Extract the message from the buffer.
			std::string message;
			std::getline(read_stream_, message, '\n');
			
			// The client said something, so he can't be dead.
			dead_timer_.cancel();
			
			if (message == "Ding Dong!") {
				server_.sendMessage("Ding Dong!");
			}
			
			if (server_.message_handler) server_.message_handler(*this, message);
			
			asyncReadMessage_();
			
		}
	}


	/// Start a new heartbeat.
	void Connection::startHeartbeatTimer_() {
		auto handler = [this] (ErrorCode const & error) {
			handleHeartbeat_(error);
		};
		heartbeat_timer_.expires_from_now(heartbeat_timeout);
		heartbeat_timer_.async_wait(handler);
	}


	/// Start the dead timer.
	void Connection::startDeadTimer_() {
		auto handler = [this] (ErrorCode const & error) {
			handleDead_(error);
		};
		dead_timer_.expires_from_now(dead_timeout);
		dead_timer_.async_wait(handler);
	}


	/// Handle a heartbeat timer expire.
	/**
	 * \param error Describes the error that occured, if any did.
	 */
	void Connection::handleHeartbeat_(ErrorCode const & error) {
		if (error) {
			// What could possibly go wrong?
		} else {
			// Ask the client if it's alive.
			sendMessage("Alive?");
			
			// Start the timer to check if the client still lives.
			startDeadTimer_();
			
			// Start a new heartbeat.
			startHeartbeatTimer_();
		}
	}


	/// Handle a dead timer expire.
	/**
	 * \param error Describes the error that occured, if any did.
	 */
	void Connection::handleDead_(ErrorCode const & error) {
		// If the timer didn't get canceled, the connection is dead.
		if (error != boost::asio::error::operation_aborted) {
			destroy_();
		}
	}


	/// Destroy the connection in a safe manner.
	/**
	 * This method closes the socket first, and then posts a task
	 * to the ASIO IO service to destroy the connection.
	 * This way, all queued handlers will execute first and can
	 * still safely use the connection object.
	 */
	void Connection::destroy_() {
		if (socket_.is_open()) {
			socket_.close();
		}
		if (!death_imminent_.test_and_set()) {
			auto erase = [this](){
				server_.connections_.erase(index_);
			};
			server_.ios().post(erase);
		}
	}
}

