#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include "connection.hpp"
#include "client.hpp"
#include "server.hpp"


namespace ascf {
	
	/// Protocol Buffers protocol.
	template<typename ClientMessageT, typename ServerMessageT, typename TransportT = boost::asio::ip::tcp>
	struct ProtocolBuffers {
		typedef ProtocolBuffers<ClientMessageT, ServerMessageT, TransportT> Protocol;
		typedef ServerMessageT                              ServerMessage;
		typedef ClientMessageT                              ClientMessage;
		typedef TransportT                                  Transport;
		typedef ::ascf::ServerExtension<Protocol>           ServerExtension;
		typedef ::ascf::ServerConnectionExtension<Protocol> ServerConnectionExtension;
		typedef ::ascf::ClientExtension<Protocol>           ClientExtension;
		
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
			buffer->push_back((size >> 3 * 8) & 0xff);
			buffer->push_back((size >> 2 * 8) & 0xff);
			buffer->push_back((size >> 1 * 8) & 0xff);
			buffer->push_back((size >> 0 * 8) & 0xff);
			
			// Add the message.
			message.AppendToString(buffer.get());
			
			return buffer;
		}
		
		/// Consume a message from the buffer.
		/**
		 * \param buffer The buffer to read from.
		 * \return The message, if the buffer contained a valid message.
		 */
		template<typename MessageType>
		static boost::optional<MessageType> consumeMessage(boost::asio::streambuf & buffer) {
			// Buffer must contain atleast 4 bytes, the length of the message.
			if (buffer.size() < 4) return false;
			
			// Read message size in big endian.
			char const * data = boost::asio::buffer_cast<char const *>(buffer.data());
			unsigned int size =
				  (data[0] << 3 * 8)
				+ (data[1] << 2 * 8)
				+ (data[2] << 1 * 8)
				+ (data[3] << 0 * 8);
			
			// Size of 0 means an empty (default constructed).
			if (!size) {
				buffer.consume(4);
				return boost::make_optional(MessageType());
				
			// Make sure buffer contains the entire message.
			} else if (buffer.size() >= 4 + size) {
				boost::optional<MessageType> message((MessageType()));
				message->ParseFromArray(reinterpret_cast<void const *>(&data[4]), size);
				buffer.consume(4 + size);
				return message;
			// Buffer didn't contain a complete message.
			} else {
				return boost::optional<MessageType>();
			}
		}
	};
	
}
