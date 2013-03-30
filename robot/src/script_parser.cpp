#include <memory>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "script_parser.hpp"
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
	ScriptParser::ScriptParser(command::Factory & factory) :
		factory_(factory)
	{
		reset();
	}
	
	/// Reset the parser so that it can parse a new command.
	void ScriptParser::reset() {
		state_ = State::text;
		
		root_     = std::make_shared<command::Execute>(nullptr);
		sentence_ = std::make_shared<command::Speech>(root_.get());
		
		command_name_.clear();
		command_args_.clear();
		current_arg_.clear();
		level_ = 0;
	}
	
	/// Get the parse result.
	/**
	 * May throw an exception if the parser is not in a valid state to return a result.
	 * If no exception is thrown, the parser is reset as if a call to reset() has been made.
	 * 
	 * \return A shared pointer holding the parsed executable.
	 */
	command::SharedPtr ScriptParser::result() {
		// Make sure the parser isn't in the middle of something.
		if (state_ != State::text) {
			throw std::runtime_error("Parser requires more input before returning a result.");
		}
		
		// Flush the final sentence.
		if (sentence_->text.size()) flushSentence_();
		
		command::SharedPtr result;
		// If we read exactly one command, just return that instead.
		if (root_->children.size() == 1) {
			result = root_->children[0];
			result->parent = nullptr;
		// Otherwise, return the aggregate command.
		} else {
			result = root_;
		}
		
		reset();
		return result;
	}
	
	/// Parse one character of input.
	/**
	 * \param c The input character.
	 * \return bool True if the parser is done.
	 */
	bool ScriptParser::consume(char c) {
		switch (state_) {
			// Parsing normal text.
			case State::text:
				// An opening curly bracket starts a command.
				if (c == '{') {
					state_ = State::command_name;
					return false;
					
				// Ignore whitespace as long as the sentence is empty.
				} else if (isSpace(c) && !sentence_->text.size()) {
					return false;
					
				// The rest is text.
				} else {
					sentence_->text.push_back(c);
					if (endsSentence(c)) {
						flushSentence_();
					}
					return false;
				}
				
			// Parsing a command name.
			case State::command_name:
				// Pipe symbol starts the argument list.
				if (c == '|') {
					state_ = State::command_args;
					level_ = 0;
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
					
				// Fold any whitespace in the middle of a name into a single space.
				} else if (isSpace(c)) {
					if (command_name_.size() && command_name_.back() != ' ') {
						command_name_.push_back(' ');
					}
					return false;
					
				// Anything else is bogus.
				} else {
					throw std::runtime_error("Illegal character encountered in command name.");
				}
				
			// Gather arguments.
			case State::command_args:
				// Closing curly bracket on level 0 ends the command.
				if (c == '}' && !level_) {
					flushArgument_();
					flushCommand_();
					state_ = State::text;
					return false;
				// Pipe symbol on level 0 ends the argument.
				} else if (c == '|' && !level_) {
					flushArgument_();
					return false;
				// Opening curly bracket increases the level.
				} else if (c == '{') {
					++level_;
					current_arg_.push_back(c);
					return false;
				// Closing curly bracket increases the level.
				} else if (c == '}') {
					--level_;
					current_arg_.push_back(c);
					return false;
				// Anything else is just input.
				} else {
					current_arg_.push_back(c);
					return false;
				}
				
			// Can't happen, but g++ won't shut up about it.
			default:
				throw std::logic_error("Parser in invalid state.");
		}
	}
	
	/// Flush the last read sentence
	void ScriptParser::flushSentence_() {
		root_->children.push_back(sentence_);
		sentence_ = std::make_shared<command::Speech>(root_.get());
	}
	
	/// Flush the last read command argument.
	void ScriptParser::flushArgument_() {
		command_args_.push_back(current_arg_);
		current_arg_.clear();
		level_ = 0;
	}
	
	/// Flush the recently parsed command.
	void ScriptParser::flushCommand_() {
		trim(command_name_);
		if (sentence_->text.size()) {
			sentence_->children.push_back(factory_.create(sentence_.get(), std::move(command_name_), std::move(command_args_)));
			sentence_->text += "\\mrk=" + boost::lexical_cast<std::string>(sentence_->children.size()) + "\\";
		} else {
			root_->children.push_back(factory_.create(root_.get(), std::move(command_name_), std::move(command_args_)));
		}
		
		command_name_.clear();
		command_args_.clear();
	}
	
}
