#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>


namespace ascf {
	
	/// Protocol Bufffers protocol.
	template<typename ClientMessageT, typename ServerMessageT, typename TransportT = boost::asio::ip::tcp>
	struct ProtocolBuffers {
		typedef ServerMessageT                ServerMessage;
		typedef ClientMessageT                ClientMessage;
		typedef TransportT                    Transport;
		typedef typename Transport::socket    Socket;
		typedef typename Transport::acceptor  Acceptor;
		typedef typename Transport::endpoint  Endpoint;
		
		/// Frame a message by prefixing it with it's size.
		/**
		 * \param message The message to frame.
		 * \return A buffer holding the message size in big endian, followed by the serialized message.
		 */
		template<typename MessageType>
		static std::shared_ptr<std::string> frameMessage(MessageType const & message) {
			int size = message.ByteSize();
			auto buffer = std::make_shared<std::string>();
			buffer->reserve(4 + size);
			
			// Add size in big endian.
			buffer->push_back((size >> (3 * 8)) & 0xff);
			buffer->push_back((size >> (2 * 8)) & 0xff);
			buffer->push_back((size >> (1 * 8)) & 0xff);
			buffer->push_back((size >> (0 * 8)) & 0xff);
			
			// Add the message.
			message.AppendToString(buffer.get());
			
			return buffer;
		}
		
		/// Consume a character from the buffer and pass it to a handler.
		/**
		 * \param buffer The buffer to read from.
		 * \param handler The handler to pass messages to.
		 * \return True if a message was consumed.
		 */
		template<typename MessageType>
		static bool consumeMessage(boost::asio::streambuf & buffer, std::function<void (MessageType &&)> handler) {
			// Buffer must contain atleast 4 bytes, the length of the message.
			if (buffer.size() < 4) return false;
			
			// Read message size in big endian.
			char const * data = boost::asio::buffer_cast<char const *>(buffer.data());
			unsigned int size =
				  (data[0] << (3 * 8))
				+ (data[1] << (2 * 8))
				+ (data[2] << (1 * 8))
				+ (data[3] << (0 * 8));
			
			// Size of 0 means there was not actually a message.
			if (!size) {
				buffer.consume(4);
				return true;
			// Make sure buffer contains the entire message.
			} else if (buffer.size() >= 4 + size) {
				MessageType message;
				message.ParseFromArray(reinterpret_cast<void const *>(&data[4]), size);
				buffer.consume(4 + size);
				handler(std::move(message));
				return true;
			// Buffer didn't contain a complete message.
			} else {
				return false;
			}
		}
	};
	
}
