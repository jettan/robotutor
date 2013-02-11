#include "script_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	/// Construct an executable parser.
	/**
	 * \param factory The command factory to use for encountered commands.
	 */
	ExecutableParser::ExecutableParser(executable::CommandFactory & factory) : text_parser_(factory) {};
	
	/// Get the parse result.
	/**
	 * May only be called after a call to finish().
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	executable::SharedPtr ExecutableParser::result() {
		return result_;
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool ExecutableParser::consume(char c) {
		return text_parser_.consume(c);
	}
	
	/// Inform the parser that there is no more input.
	/**
	 * \return True if the parser is in a valid stop state.
	 */
	bool ExecutableParser::finish() {
		if (!text_parser_.finish()) return false;
		result_ = text_parser_.result();
		return true;
	}

}
