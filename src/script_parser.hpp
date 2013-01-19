#include <istream>
#include <streambuf>
#include <string>
#include <vector>
#include <stdexcept>
#include <iterator>

#ifndef ROBOTUTOR_SCRIPT_PARSER_HPP_
#define ROBOTUTOR_SCRIPT_PARSER_HPP_

#include "executable.hpp"

namespace robotutor {
	/// Parser for executables.
	/**
	 * This parser will read the next executable from the input.
	 */
	class ExecutableParser {
		public:
			executable::SharedPtr executable;
			bool consume(char c);
	};
	
	/// Parser for commands.
	/**
	 * Reads one command starting with '{'  and ending with '}'.
	 */
	class CommandParser {
		protected:
			/// Buffer for the command name.
			std::string name_;
			
			/// Buffer for arguments.
			executable::ArgList args_;
			
			/// Parser for arguments.
			ExecutableParser arg_parser_;
			
			/// State of the parser.
			enum {
				STATE_START,
				STATE_NAME,
				STATE_ARGS
			} state_;
			
		public:
			/// The resulting command.
			executable::SharedPtr command;
			
			CommandParser();
			
			bool consume(char c);
			
		protected:
			void flush_();
			void flush_arg_();
	};
	
	/// Parser for text executables.
	/**
	 * This parser will read and entire text executable from the input.
	 */
	class TextParser {
		protected:
			enum {
				STATE_TEXT,
				STATE_COMMAND
			} state_;
			
			CommandParser arg_parser_;
			
		public:
			executable::Text text;
			
			TextParser();
			
			bool consume(char c);
	};
	
	/// Parse an input sequence.
	/**
	 * \param parser The parser to use.
	 * \param begin An input iterator denoting the start of the input sequence.
	 * \param end An input iterator denoting the end of the input sequence.
	 * \return True if the parser has finished processing input.
	 */
	template<typename Parser, typename InputIterator>
	bool parse(Parser & parser, InputIterator start, InputIterator end) {
		while (start != end) {
			if (parser.consume(*start++)) return true;
		}
		return false;
	}
	
	/// Parse a string.
	/**
	 * \param parser The parser to use.
	 * \param input The string.
	 * \return True if the parser has finished processing input.
	 */
	template<typename Parser>
	bool parse(Parser & parser, std::string const & input) {
		return parse(parser, input.begin(), input.end());
	}
	
	/// Parse a stream buffer.
	/**
	 * \param parser The parser to use.
	 * \param streambuf The stream buffer.
	 * \return True if the parser has finished processing input.
	 */
	template<typename Parser>
	bool parse(Parser & parser, std::streambuf & streambuf) {
		return parse(parser, std::istreambuf_iterator<char>(streambuf), std::istreambuf_iterator<char>());
	}
	
	/// Parse an input stream.
	/**
	 * \param parser The parser to use.
	 * \param stream The input stream.
	 * \return True if the parser has finished processing input.
	 */
	template<typename Parser>
	bool parse(Parser & parser, std::istream stream) {
		return parse(parser, *stream.rdbuf());
	}
	
}

#endif
