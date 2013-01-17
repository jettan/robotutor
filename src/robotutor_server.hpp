#include <boost/asio.hpp>
#include <boost/bind.hpp>

#ifndef ROBOTUTOR_SERVER_HPP_

namespace robotutor {
	class Connection{
		public:
			typedef boost::shared_ptr<Connection> SharedPtr;
			typedef boost::asio::ip::tcp::socket Socket;
			
		protected:
			Socket socket_;
			Connection(boost::asio::io_service & ios) : socket_(ios) {};
			
		public:
			static pointer create(boost::asio::io_service & ios) {
				return pointer(new Connection(ios));
			}
			
			Socket & socket() {
				return socket_;
			}
			
			void start();
	};
	
	class Server {
		public:
			typedef boost::asio::ip::tcp::acceptor Acceptor;
			typedef boost::asio::ip::tcp::endpoint Endpoint;
			
		protected:
			Acceptor acceptor_;
			
		public:
			Server(boost::asio::io_service & ios) : acceptor_(ios, Endpoint(boost::asio::ip::tcp::v4(), 2854)) {
				start_accept();
			}
			
		protected:
			void start_accept();
			void handle_accept(Connection::pointer new_connection, const boost::system::error_code & error);
	};
}

