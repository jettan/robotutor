#pragma once
#include <istream>
#include <streambuf>
#include <string>
#include <iterator>


namespace robotutor {
	
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
		return parse(parser, std::istreambuf_iterator<char>(&streambuf), std::istreambuf_iterator<char>());
	}
	
	/// Parse an input stream.
	/**
	 * \param parser The parser to use.
	 * \param stream The input stream.
	 * \return True if the parser has finished processing input.
	 */
	template<typename Parser>
	bool parse(Parser & parser, std::istream & stream) {
		return parse(parser, *stream.rdbuf());
	}
	
}
