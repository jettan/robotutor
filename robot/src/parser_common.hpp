#include <string>

#ifndef ROBOTUTOR_PARSER_COMMON_HPP_
#define ROBOTUTOR_PARSER_COMMON_HPP_

namespace robotutor {
	
	namespace parser {
		
		/// Check if a character is a lowercase alphabetical characters.
		/**
		 * \param c The character to check.
		 * \return True if the character is a lowercase alphabetical characters.
		 */
		inline bool isLowerAlpha(char const c) {
			return c >= 'a' && c <= 'z';
		}
		
		/// Check if a character is an uppercase alphabetical characters.
		/**
		 * \param c The character to check.
		 * \return True if the character is an uppercase alphabetical characters.
		 */
		inline bool isUpperAlpha(char const c) {
			return c >= 'A' && c <= 'Z';
		}
		
		/// Check if a character is an alphabetical characters.
		/**
		 * \param c The character to check.
		 * \return True if the character is an alphabetical characters.
		 */
		inline bool isAlpha(char const c) {
			return isLowerAlpha(c) || isUpperAlpha(c);
		}
		
		/// Check if a character is a digit.
		/**
		 * \param c The character to check.
		 * \return True if the character is a digit.
		 */
		inline bool isDigit(char c) {
			return c >= '0' && c <= '9';
		}
		
		/// Check if a character is whitespace.
		/**
		 * \param c The character to check.
		 * \return True if the character is whitespace.
		 */
		inline bool isSpace(char c) {
			return c == ' ' || c == '\t' || c == '\n' || c == '\r';
		}
		
		/// Trim leading spaces from a string.
		/**
		 * \param input The string.
		 */
		inline void triml(std::string & s) {
			std::string::iterator i;
			for (i = s.begin();
				 i != s.end() && isSpace(*i);
				 ++i);
			s.erase(s.begin(), i);
		}
		
		/// Trim trailing spaces from a string.
		/**
		 * \param input The string.
		 */
		inline void trimr(std::string & s) {
			std::string::reverse_iterator ri;
			for (ri = s.rbegin();
				 ri != s.rend() && isSpace(*ri);
				 ++ri);
			s.erase(ri.base(), s.end());
		}
		
		/// Trim trailing and leading spaces from a string.
		/**
		 * \param input The string.
		 */
		inline void trim(std::string & s) {
			triml(s);
			trimr(s);
		}
	}
}

#endif
