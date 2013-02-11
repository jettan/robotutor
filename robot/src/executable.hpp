#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#ifndef ROBOTUTOR_EXECUTABLE_HPP_
#define ROBOTUTOR_EXECUTABLE_HPP_

namespace robotutor {
	
	class ScriptEngine;
	
	namespace executable {
		
		class Executable;
		typedef boost::shared_ptr<Executable> SharedPtr;
		typedef std::vector<SharedPtr> ArgList;
		
		/// Base class for executables.
		class Executable {
			public:
				/// Arguments for the executable.
				ArgList arguments;
				
				/// Virtual destructor.
				virtual ~Executable() {};
				
				/// Execute one step.
				virtual bool step(ScriptEngine & engine) = 0;
			
		};
		
		/// Executable text.
		/**
		 * The normal text is synthesized by a TTS engine,
		 * the executables are executed when a bookmark is encountered.
		 */
		struct Text : public Executable {
			/// Shared pointer type.
			typedef boost::shared_ptr<Text> SharedPtr;
			
			/// The text to synthesize.
			std::string text;
			
			bool step(ScriptEngine & engine);
		};
		
		
		/// Base class for executable commands.
		struct Command : public Executable {
			/// Shared pointer type.
			typedef boost::shared_ptr<Command> SharedPtr;
			
			/// The name of the command.
			std::string name;
		};
		
		/// Write a text executable to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Text const & text);
		
		/// Write a command to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Command const & command);
		
		/// Write any executable to a stream.
		/**
		 * \param stream The stream to write to.
		 * \param text   The text command to write.
		 */
		std::ostream & operator << (std::ostream & stream, executable::Executable const & command);
		
		/// Create a command from a name and an argument list.
		class CommandFactory {
			protected:
				/// Function type for creator functions.
				typedef boost::function<Command::SharedPtr (std::string const &, ArgList const & arguments)> Creator;
				
				/// Map type for the creator map.
				typedef std::map<std::string, Creator> CreatorMap;
				
				/// Map holding creator functions.
				CreatorMap creators_;
				
			public:
				
				/// Create a command from a name and argument list.
				Command::SharedPtr create(std::string const & name, ArgList const & args) {
					CreatorMap::iterator creator = creators_.find(name);
					if (creator == creators_.end()) throw std::runtime_error("Command `" + name + "' not found.");
					return creator->second(name, args);
				}
				
				/// Register a creator.
				void add(std::string const & name, Creator creator) {
					creators_[name] = creator;
				}
		};
		
	}
}

#endif
