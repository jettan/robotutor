#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#ifndef ROBOTUTOR_EXECUTABLE_HPP_
#define ROBOTUTOR_EXECUTABLE_HPP_

namespace robotutor {
	
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
				virtual bool step() = 0;
			
		};
		
		/// Executable text.
		/**
		 * The normal text is synthesized by a TTS engine,
		 * the executables are executed when a bookmark is encountered.
		 */
		struct Text : public Executable {
			/// The text to synthesize.
			std::string text;
			
			bool step();
		};
		
		class Command : public Executable {
			public:
				std::string name;
				bool step();
		};
		
		/// Create a command from a name and an argument list.
		class CommandFactory {
			protected:
				/// Function type for creator functions.
				typedef boost::function<SharedPtr (ArgList const & arguments)> creator;
				
				/// Map type for the creator map.
				typedef std::map<std::string, creator> CreatorMap;
				
				/// Map holding creator functions.
				static CreatorMap creators;
				
			public:
				
				/// Create a command from a name and argument list.
				SharedPtr static create(std::string name, ArgList args) {
					CreatorMap::iterator creator = creators.find(name);
					if (creator == creators.end()) throw std::runtime_error("Command `" + name + "' not found.");
					return creator->second(args);
				}
		};
		
	}
}

#endif
