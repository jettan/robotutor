#include <string>
#include <stdexcept>
#include <memory>

#include <boost/lexical_cast.hpp>

#include "../script_engine.hpp"
#include "../command.hpp"
#include "../command_factory.hpp"
#include "../messages.pb.h"

namespace robotutor {
	namespace command {
		
		/// Interface for turningpoint commands.
		class TurningPoint : public Command {
			
			protected:
				/// Indicates whether a result has been requested.
				bool results_requested_ = false;
				
			public:
				/// Construct the command.
				/**
				 * \param arguments The arguments for the command.
				 */
				TurningPoint(Command * parent) :
					Command(parent) {}
				
				virtual void processResults(TurningPointMessage & results) = 0;
				
			protected:
				/// Request turningpoint results from server.
				void requestResults_(ScriptEngine & engine) {
					results_requested_= true;
					RobotMessage message;
					message.set_fetch_turningpoint(true);
					engine.server.sendMessage(message);
				}
		};
		
		/// Command to run a turningpoint choice.
		struct TurningPointChoice : public TurningPoint {
			/// Construct the command.
			/**
			 * \param arguments The arguments for the command.
			 */
			TurningPointChoice(Command * parent) :
				TurningPoint(parent) {}
			
			/// Create the command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				auto result = std::make_shared<Execute>(parent);
				for (auto const & argument : arguments) {
					result->arguments.push_back(parseScript(factory, argument));
					result->arguments.back()->parent = result.get();
				}
				return result;
			}
			
			/// The name of the command.
			static std::string static_name() { return "turningpoint choice"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return TurningPointChoice::static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine) {
				if (!results_requested_) {
					requestResults(engine);
					return false;
				}
				return done_(engine);
			}
		};
		
		/// Command to run a turningpoint quiz.
		struct TurningPointQuiz : public TurningPoint {
			/// The index of the correct answer.
			unsigned int correct_index;
			
			/// The correct answer as string.
			std::string correct_answer;
			
			/// The flag to decide if we use an index or a string.
			bool use_string;
			
			/// Construct the command.
			/**
			 * \param arguments The arguments for the command.
			 */
			TurningPointQuiz(Command * parent, unsigned int index, std::string const & answer, bool use_string) :
				TurningPoint(parent),
				correct_index(index),
				correct_answer(answer),
				use_string(use_string) {}
			
			/// Create the command.
			static SharedPtr create(Command * parent, std::string && name, std::vector<std::string> && arguments, Factory &) {
				if (arguments.size() < 2 || arguments.size() > 4) throw std::runtime_error("TurningPointQuiz command expects 2-4 arguments!");
				unsigned int index;
				std::string answer;
				bool use_string = false;
				
				try {
					index = boost::lexical_cast<unsigned int>(arguments[0]);
				} catch (boost::bad_lexical_cast const &) {
					use_string = true;
					answer     = arguments[0];
				}
				
				auto result = std::make_shared<Execute>(parent, index, answer, use_string);
				for (auto argument = arguments.begin()+1; argument != arguments.end(); ++argument) {
					result->arguments.push_back(parseScript(factory, *argument));
					result->arguments.back()->parent = result.get();
				}
				return result;
			}
			
			/// The name of the command.
			static std::string static_name() { return "turningpoint quiz"; }
			
			/// Get the name of the command.
			/**
			 * \return The name of the command.
			 */
			std::string name() const { return TurningPointQuiz::static_name(); }
			
			/// Run the command.
			/**
			 * \param engine The script engine to use for executing the command.
			 */
			bool step(ScriptEngine & engine) {
				if (!results_requested_) {
					requestResults(engine);
					return false;
				}
				return done_(engine);
			}
		};
		
		/// Load the commands in a factory.
		/**
		 * Load all commands of a plugin into a factory.
		 * 
		 * \param factory The factory to add the commands to.
		 */
		extern "C" void loadCommands(Factory & factory) {
			factory.add<TurningPointChoice>();
			factory.add<TurningPointQuiz>();
		}
	}
}

