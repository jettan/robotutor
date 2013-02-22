#include <memory>

#include <boost/lexical_cast.hpp>

#include "command_parser.hpp"
#include "parser_common.hpp"

namespace robotutor {
	
	using namespace parser;
	
	namespace {
		/// Check if a character ends a sentence.
		/**
		 * \param c The character to check.
		 * \return True if the character ends a sentence.
		 */
		bool endsSentence(char c) {
			switch (c) {
				case '.':
				case '!':
				case '?':
				case ';':
					return true;
				default:
					return false;
			}
		}
	}
	
	/// Construct a text parser.
	CommandParser::CommandParser(command::Factory & factory) : factory_(factory) {
		reset();
	}
	
	/// Reset the parser so that it can parse a new command.
	void CommandParser::reset() {
		state_            = State::text;
		sentenceNonEmpty_ = false;
		textNonEmpty_     = false;
		
		command_name_.clear();
		command_args_.clear();
		
		text_.sentences.clear();
		text_.arguments.clear();
		text_.marks.clear();
		
		text_.sentences.push_back(std::string());
		text_.marks.push_back(0);
	}
	
	/// Get the parse result.
	/**
	 * May throw an exception if the parser is not in a valid state to return a result.
	 * If no exception is thrown, the parser is reset as if a call to reset() has been made.
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	command::SharedPtr CommandParser::result() {
		// Make sure the parser isn't in the middle of something.
		if (state_ != State::text && state_ != State::done) {
			throw std::runtime_error("Parser requires more input before returning a result.");
		}
		
		// Flush the final sentence and remove the last empty sentence.
		flushSentence_();
		text_.sentences.pop_back();
		text_.marks.pop_back();
		
		// Trim the original text.
		trim(text_.original);
		
		command::SharedPtr result;
		// If we read exactly one command, just return that instead.
		if (!textNonEmpty_ && text_.arguments.size() == 1) {
			result = text_.arguments[0];
		// Otherwise, return a real text command.
		} else {
			result = std::make_shared<command::Text>(std::move(text_));
		}
		
		reset();
		return result;
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool CommandParser::consume(char c) {
		
		text_.original.push_back(c);
		
		switch (state_) {
			// Parsing normal text.
			case State::text:
				// An opening curly bracket starts a command.
				if (c == '{') {
					state_ = State::command_name;
					return false;
					
				// A colon or closing curly bracket ends the text.
				} else if (c == '|' || c == '}') {
					state_ = State::done;
					return true;
					
				// The rest is text.
				} else {
					text_.sentences.back().push_back(c);
					sentenceNonEmpty_ = sentenceNonEmpty_ || !isSpace(c);
					if (endsSentence(c)) flushSentence_();
					return false;
				}
				
			// Parsing a command name.
			case State::command_name:
				// Pipe symbol starts the argument list.
				if (c == '|') {
					state_ = State::command_args;
					arg_parser_.reset(new CommandParser(factory_));
					return false;
					
				// A closing curly bracket closes the command.
				} else if (c == '}') {
					flushCommand_();
					state_ = State::text;
					return false;
					
				// Lower case alpha characters, digits and normal spaces can be part of the name.
				} else if (isLowerAlpha(c) || isDigit(c)) {
					command_name_.push_back(c);
					return false;
					
				// Fold any whitespace into a single space.
				} else if (isSpace(c)) {
					if (!command_name_.size() || !isSpace(command_name_.back())) {
						command_name_.push_back(' ');
					}
					return false;
					
				// Anything else is bogus.
				} else {
					throw std::runtime_error("Illegal character encountered in command name.");
				}
				
			// Parsing command arguments in sub-parser.
			case State::command_args:
				if (arg_parser_->consume(c)) {
					command_args_.push_back(arg_parser_->result());
					if (c == '}') {
						flushCommand_();
						state_ = State::text;
						return false;
					} else if (c == '|') {
						arg_parser_.reset(new CommandParser(factory_));
						return false;
					} else {
						throw std::runtime_error("Unexpected character finished command argument.");
					}
				} else {
					return false;
				}
				
			// Done parsing, stop feeding us.
			case State::done:
				throw std::runtime_error("Input received after command parsing finished.");
				
			// Can't happen, but G++ won't shut up about it.
			default:
				throw std::logic_error("Parser in invalid state.");
		}
	}
	
	/// Flush the last read sentence
	void CommandParser::flushSentence_() {
		textNonEmpty_ = textNonEmpty_ || sentenceNonEmpty_;
		// Only add a new sentence if the last one is non empty.
		if (sentenceNonEmpty_) {
			text_.sentences.push_back(std::string());
			text_.marks.push_back(text_.arguments.size());
		}
		sentenceNonEmpty_ = false;
	}
	
	/// Flush the recently parsed command.
	void CommandParser::flushCommand_() {
		trim(command_name_);
		text_.arguments.push_back(factory_.create(std::move(command_name_), std::move(command_args_)));
		
		// If the sentence is non-empty, add a bookmark for the command.
		if (sentenceNonEmpty_) {
			text_.sentences.back() += "\\mrk=" + boost::lexical_cast<std::string>(text_.arguments.size()) + "\\";
		// If the sentence is empty, bump the mark number to indicate the command should be executed before the sentence.
		} else {
			text_.marks.back() = text_.arguments.size();
		}
		
		command_name_.clear();
		command_args_.clear();
	}
	
}
