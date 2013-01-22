#include <istream>
#include <streambuf>
#include <string>
#include <vector>
#include <stdexcept>
#include <iterator>

#include <boost/scoped_ptr.hpp>

#ifndef ROBOTUTOR_SCRIPT_PARSER_HPP_
#define ROBOTUTOR_SCRIPT_PARSER_HPP_

#include "executable.hpp"

namespace robotutor {
	
	class TextParser;
	class CommandParser;
	class ExecutableParser;
	
	/// Parser for text executables.
	/**
	 * This parser will read an entire text executable from the input.
	 */
	class TextParser {
		protected:
			/// The state of the parser.
			enum {
				STATE_TEXT,
				STATE_COMMAND
			} state_;
			
			/// Parser for embedded commands.
			boost::scoped_ptr<CommandParser> arg_parser_;
			
			/// The text executable.
			boost::shared_ptr<executable::Text> result_;
			
		public:
			/// Construct a text parser.
			TextParser();
			
			/// Get the parse result.
			/**
			 * May only be called after a call to finish().
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			boost::shared_ptr<executable::Text> result();
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
			/// Inform the parser that there is no more input.
			/**
			 * \return True if the parser is in a valid stop state.
			 */
			bool finish();
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
			boost::scoped_ptr<ExecutableParser> arg_parser_;
			
			/// State of the parser.
			enum {
				STATE_START,
				STATE_NAME,
				STATE_ARGS,
				STATE_DONE
			} state_;
			
			/// The resulting command.
			executable::SharedPtr result_;
			
		public:
			/// Construct a command parser.
			CommandParser();
			
			/// Get the parse result.
			/**
			 * May only be called after a call to finish().
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			executable::SharedPtr result();
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
			/// Inform the parser that there is no more input.
			/**
			 * \return True if the parser is in a valid stop state.
			 */
			bool finish();
			
		protected:
			/// Flush the command name buffer.
			void flush_name_();
			/// Flush the argument buffer.
			void flush_arg_();
	};
	
	/// Parser for executables.
	/**
	 * This parser will read the next executable from the input.
	 */
	class ExecutableParser {
		protected:
			/// Parser that secretly does all the work.
			TextParser text_parser_;
			
			/// Shared pointer to hold the result.
			executable::SharedPtr result_;
			
		public:
			/// Get the parse result.
			/**
			 * May only be called after a call to finish().
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			executable::SharedPtr result();
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
			/// Inform the parser that there is no more input.
			/**
			 * \return True if the parser is in a valid stop state.
			 */
			bool finish();
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
