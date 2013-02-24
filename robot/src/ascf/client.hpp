#pragma once

#include <functional>
#include <memory>

#include <boost/asio.hpp>

#include "connection.hpp"

namespace ascf {
	
	template<typename Protocol>
	class Client : public Connection<Protocol, false> {
		public:
			typedef Client<Protocol>                         ConnectionType;
			typedef Connection<Protocol, false>              ParentType;
			typedef boost::system::error_code                ErrorCode;
			
			typedef std::function<void(std::shared_ptr<ConnectionType> connection, typename Protocol::ServerMessage && message)> MessageHandler;
			
			/// The message handler.
			MessageHandler message_handler;
			
			/// Construct a client connection.
			/**
			 * \param must_be_shared Token to prevent direct construction.
			 * \param ios The IO service to use.
			 */
			Client(typename ParentType::must_be_shared_ must_be_shared, boost::asio::io_service & ios) :
				ParentType(must_be_shared, typename Protocol::Socket(ios)) {}
			
			/// Virtual deconstructor.
			virtual ~Client() {}
			
			/// Connect to an endpoint.
			/**
			 * \param endpoint The endpoint to connect to.
			 */
			void connect(typename Protocol::Endpoint const & endpoint) {
				auto handler = [this] (ErrorCode const & error) {
					handleConnect_(error);
				};
				
				this->socket_.async_connect(endpoint, handler);
			}
			
			/// Create a new server connection.
			/**
			 * \param ios The IO service to use.
			 * \return A shared pointer to a new server connection.
			 */
			static std::shared_ptr<ConnectionType> create(boost::asio::io_service & ios) {
				return std::make_shared<ConnectionType>(typename ParentType::must_be_shared_(), ios);
			}
			
		protected:
			/// Get a shared pointer to this client.
			/**
			 * \return A shared pointer to this client.
			 */
			std::shared_ptr<ConnectionType> get_shared_() {
				return std::static_pointer_cast<ConnectionType>(this->shared_from_this());
			}
			
			/// Handle an asynchronous connect event.
			/**
			 * \param error The error that occured, if any.
			 */
			void handleConnect_(ErrorCode const & error) {
				if (!error) {
					this->asyncRead_();
				}
			}
			
			/// Handle messages by passing them to the user defined message handler.
			/**
			 * \param message The message to handle.
			 */
			virtual void handleMessage_(typename Protocol::ServerMessage && message) {
				if (message_handler) message_handler(get_shared_(), std::forward<typename Protocol::ServerMessage>(message));
			}
			
	};
	
	
}
