#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include <boost/asio.hpp>

#include "connection.hpp"

namespace ascf {
	
	template<typename ProtocolT>
	class Client : public Connection<ProtocolT, false> {
		
		public:
			typedef ProtocolT                                Protocol;
			typedef Connection<Protocol, false>              BaseType;
			typedef boost::system::error_code                ErrorCode;
			
			typedef std::function<void(std::shared_ptr<Client<Protocol>> connection, typename Protocol::ServerMessage && message)> MessageHandler;
			
		protected:
			/// Protocol defined extension.
			typename Protocol::ClientExtension extension_;
			
		public:
			/// The message handler.
			MessageHandler message_handler;
			
			/// Construct a client connection.
			/**
			 * \param must_be_shared Token to prevent direct construction.
			 * \param ios The IO service to use.
			 */
			Client(typename BaseType::must_be_shared_ must_be_shared, boost::asio::io_service & ios) :
				BaseType(must_be_shared, typename Protocol::Transport::socket(ios)),
				extension_(*this) {}
			
			/// Virtual deconstructor.
			~Client() {}
			
			/// Get the protocol defined extension object.
			typename Protocol::ClientExtension       & extension()       { return extension_; };
			/// Get the protocol defined extension object.
			typename Protocol::ClientExtension const & extension() const { return extension_; };
			
			/// Create a new client connection.
			/**
			 * \param ios The IO service to use.
			 * \return A shared pointer to a new client connection.
			 */
			static std::shared_ptr<Client<Protocol>> create(boost::asio::io_service & ios) {
				return std::make_shared<Client<Protocol>>(typename BaseType::must_be_shared_(), ios);
			}
			
			/// Connect to an endpoint.
			/**
			 * \param endpoint The endpoint to connect to.
			 */
			void connect(typename Protocol::Transport::endpoint const & endpoint) {
				auto handler = [this] (ErrorCode const & error) {
					handleConnect_(error);
				};
				
				this->socket_.async_connect(endpoint, handler);
			}
			
			/// Close the connection.
			void close() {
				BaseType::close();
				extension_.handleClose();
			}
			
			/// Connect to an endpoint.
			/**
			 * \param arguments The arguments for the endpoint constructor.
			 */
			template<typename... Arguments>
			void connect(Arguments... arguments) {
				connect(typename Protocol::Transport::endpoint(arguments...));
			}
			
			/// Connect to a TCP endpoint with an IPv4 address.
			/**
			 * \param address The IPv4 address.
			 * \param port    The port number.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void connect_ip4(boost::asio::ip::address_v4::bytes_type const & address, unsigned short port) {
				connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(address), port));
			}
			
			/// Connect to a TCP endpoint with an IPv4 address.
			/**
			 * \param address The IPv4 address.
			 * \param port    The port number.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void connect_ip4(std::string const & address, unsigned short port) {
				connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(address), port));
			}
			
			/// Connect to a TCP endpoint with an IPv6 address.
			/**
			 * \param address The IPv6 address.
			 * \param port    The port number.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void connect_ip6(boost::asio::ip::address_v6::bytes_type const & address, unsigned short port) {
				connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v6(address), port));
			}
			
			/// Connect to a TCP endpoint with an IPv6 address.
			/**
			 * \param address The IPv6 address.
			 * \param port    The port number.
			 */
			template<class Enable = std::enable_if<std::is_same<typename Protocol::Transport, boost::asio::ip::tcp>::value>>
			void connect_ip6(std::string const & address, unsigned short port) {
				connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v6::from_string(address), port));
			}
			
		protected:
			/// Get a shared pointer to this client.
			/**
			 * \return A shared pointer to this client.
			 */
			std::shared_ptr<Client<Protocol>> get_shared_() {
				return std::static_pointer_cast<Client<Protocol>>(this->shared_from_this());
			}
			
			/// Handle an asynchronous connect event.
			/**
			 * \param error The error that occured, if any.
			 */
			void handleConnect_(ErrorCode const & error) {
				if (!error) {
					extension_.handleConnect();
					this->asyncRead_();
				}
			}
			
			/// Handle messages by passing them to the user defined message handler.
			/**
			 * \param message The message to handle.
			 */
			void handleMessage_(typename Protocol::ServerMessage && message) {
				if (extension_.handleMessage(message) && message_handler) {
					message_handler(get_shared_(), std::forward<typename Protocol::ServerMessage>(message));
				}
			}
			
	};
	
	
}
