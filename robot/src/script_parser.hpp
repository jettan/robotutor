#pragma once
#include <string>
#include <vector>
#include <memory>

#include "parse.hpp"
#include "command_factory.hpp"
#include "command.hpp"
#include "commands/speech_commands.hpp"

namespace robotutor {
	
	/// Parser for text executables.
	/**
	 * This parser will read an entire text executable from the input.
	 */
	class ScriptParser {
		protected:
			/// The state of the parser.
			enum class State {
				text,
				command_name,
				command_args,
			} state_;
			
			/// Factory to create commands.
			command::Factory & factory_;
			
			/// Buffer for the root command.
			std::shared_ptr<command::Execute> root_;
			
			/// Sentence currently being parsed.
			std::shared_ptr<command::Speech> sentence_;
			
			/// Name of the command currently being parsed.
			std::string command_name_;
			
			/// Argument currently being read.
			std::string current_arg_;
			
			/// Arguments for the command currently being parsed.
			std::vector<std::string> command_args_;
			
			/// The level of nested commands in an argument.
			unsigned int level_;
			
		public:
			/// Construct a text parser.
			/**
			 * \param factory The command factory to use to instantiate commands found in the text.
			 */
			ScriptParser(command::Factory & factory);
			
			/// Reset the parser so that it can parse a new command.
			void reset();
			
			/// Get the parse result.
			/**
			 * May throw an exception if the parser is not in a valid state to return a result.
			 * If no exception is thrown, the parser is reset.
			 * 
			 * \return A shared pointer holding the parsed executable.
			 */
			command::SharedPtr result();
			
			/// Parse one character of input.
			/**
			 * \param c The input character.
			 * \return bool True if the parser is done.
			 */
			bool consume(char c);
			
		protected:
			/// Flush the last read sentence.
			void flushSentence_();
			
			/// Flush the last read command argument.
			void flushArgument_();
			
			/// Flush the recently parsed command.
			void flushCommand_();
	};
	
	
	/// Parse a script.
	/**
	 * \param factory The command factory to use.
	 * \param args The arguments to parse(parser, args...).
	 * \return The parsed script.
	 */
	template<typename... Args>
	command::SharedPtr parseScript(command::Factory & factory, Args&&... args) {
		ScriptParser parser(factory);
		parse(parser, std::forward<Args>(args)...);
		return parser.result();
	}
	
	
}
