#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>

#include "../plugin.hpp"
#include "../command.hpp"
#include "../script_engine.hpp"
#include "../script_parser.hpp"
#include "../robotutor_protocol.hpp"

namespace robotutor {
	namespace command {
		
		/// Interface for turningpoint commands.
		class TurningPointCommand : public Command {
			protected:
				/// Indicates whether the results have been requested.
				bool results_requested_ = false;
				
				/// True if the child command has been executed.
				bool executed_ = false;
				
				/// The branch to take.
				int branch_ = -1;
				
			public:
				TurningPointCommand(ScriptEngine & engine, Command * parent, Plugin * plugin) :
					Command(engine, parent, plugin) {}
				
				virtual ~TurningPointCommand() {}
				
				/// Process the turning point results.
				/**
				 * \param results The results.
				 */
				virtual void processResults(TurningPointResults const & results) = 0;
				
				/// Run the command.
				virtual bool step() {
					// Request results first.
					if (!results_requested_) {
						requestResults_();
						return false;
					
					// Handle the results.
					} else if (!executed_) {
						executed_ = true;
						if (branch_ >= 0 && branch_ < children.size()) {
							setNext_(children[branch_].get());
							return true;
						}
					}
					
					// No valid branch, or branch finished.
					return done_();
				}
				
			protected:
				/// Request turningpoint results from server.
				void requestResults_() {
					results_requested_= true;
					RobotMessage message;
					message.set_fetch_turningpoint(true);
					engine.server.sendMessage(message);
				}
		};
		
		/// Command to run a turningpoint choice.
		struct TurningPointChoice : public TurningPointCommand {
			
			TurningPointChoice(ScriptEngine & engine, Command * parent, Plugin * plugin) :
				TurningPointCommand(engine, parent, plugin) {}
			
			/// Create the command.
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				auto result = std::make_shared<TurningPointChoice>(engine, parent, plugin);
				for (auto const & argument : arguments) {
					result->children.push_back(parseScript(engine, argument));
					result->children.back()->parent = result.get();
				}
				return result;
			}
			
			static std::string static_name() { return "turningpoint choice"; }
			std::string name() const { return static_name(); }
			
			/// Process the turning point results.
			/**
			 * \param results The results.
			 */
			void processResults(TurningPointResults const & results) {
				int max  = 0;
				for (int i = 1; i < results.votes().size(); ++i) {
					if (results.votes().Get(i) > results.votes().Get(max)) {
						max = i;
					}
				}
				
				bool tie = false;
				for (int i = 0; i < results.votes().size(); ++i) {
					if (i!=max && results.votes().Get(i) == results.votes().Get(max)) {
						tie=true;
						break;
					}
				}
		
				// If the result is a tie, branch to the alternative branch.
				branch_ = tie ? results.votes().size() : max;
				std::cout << "Branch: " << branch_ << " taken." << std::endl;
				if (tie) std::cout << "There was also a tie btw." << std::endl;
				continue_();
			}
			
		};
		
		/// Command to run a turningpoint quiz.
		struct TurningPointQuiz : public TurningPointCommand {
			
			/// The index of the correct answer.
			int correct_index = -1;
			
			/// The correct answer as string.
			std::string correct_answer;
			
			/// If true the first argument was a string and not an int.
			bool use_string = false;
			
			TurningPointQuiz(ScriptEngine & engine, Command * parent, Plugin * plugin, unsigned int index, std::string const & answer, bool use_string) :
				TurningPointCommand(engine, parent, plugin),
				correct_index(index),
				correct_answer(answer),
				use_string(use_string) {}
			
			/// Create the command.
			static SharedPtr create(ScriptEngine & engine, Command * parent, Plugin * plugin, std::vector<std::string> && arguments) {
				if (arguments.size() < 2 || arguments.size() > 4) throw std::runtime_error("Command `" + static_name() + "' command expects 2 to 4 arguments.");
				
				int index;
				std::string answer;
				bool use_string = false;
				
				// First argument can be an int or string identifying the correct answer.
				try {
					index = boost::lexical_cast<int>(arguments[0]);
				} catch (boost::bad_lexical_cast const &) {
					use_string = true;
					answer     = arguments[0];
				}
				
				// The remaining arguments are alternatives to execute depending on the results.
				auto result = std::make_shared<TurningPointQuiz>(engine, parent, plugin, index, answer, use_string);
				for (auto argument = arguments.begin()+1; argument != arguments.end(); ++argument) {
					result->children.push_back(parseScript(engine, *argument));
					result->children.back()->parent = result.get();
				}
				return result;
			}
			
			static std::string static_name() { return "turningpoint quiz"; }
			std::string name() const { return TurningPointQuiz::static_name(); }
			
			/// Process the turning point results.
			/**
			 * \param results The results.
			 */
			void processResults(TurningPointResults const & results) override {
				// Get the total votes and top two answers.
				//int total  = std::accumulate(results.votes().begin(), results.votes().end(), 0);
				int max[2] = {0, 0};
				
				for (int i = 2; i < results.votes().size(); ++i) {
					if (results.votes().Get(i) > results.votes().Get(max[0])) {
						max[1] = max[0];
						max[0] = i;
					} else if (results.votes().Get(i) > results.votes().Get(max[1])) {
						max[1] = i;
					}
				}
				
				// Get the index of the correct answer.
				if (use_string) {
					auto i = std::find(results.answers().begin(), results.answers().end(), correct_answer);
					if (i == results.answers().end()) {
						correct_index = -1;
					} else {
						correct_index = std::distance(results.answers().begin(), i);
					}
				}
				
				// No correct answer in the results.
				// Execute nothing.
				if (correct_index < 0) {
					branch_ = -1;
					
				// Correct answer didn't win.
				// Execute the last branch.
				} else if (max[0] != correct_index) {
					branch_ = children.size();
					
				// Correct answer won.
				// Execute the first branch.
				} else if (max[0] == correct_index) {
					branch_ = 0;
				}
				
				continue_();
			}
			
		};
	}
	
	struct TurningPointPlugin : public Plugin {
		TurningPointPlugin(ScriptEngine & engine) : Plugin(engine) {
			engine.factory.add<command::TurningPointChoice>(this);
			engine.factory.add<command::TurningPointQuiz>(this);
		}
		
		void handleMessage(SharedServerConnection connection, ClientMessage const & message) override {
			if (!message.has_turningpoint()) return;
			if (auto command = dynamic_cast<command::TurningPointCommand *>(engine.current())) {
				command->processResults(message.turningpoint());
			}
		}
	};
	
	extern "C" Plugin * createPlugin(ScriptEngine & engine) {
		return new TurningPointPlugin(engine);
	}
}

