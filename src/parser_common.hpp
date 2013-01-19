#ifndef ROBOTUTOR_PARSER_COMMON_HPP_
#define ROBOTUTOR_PARSER_COMMON_HPP_

namespace robotutor {
	
	namespace parser {
		
		bool isLowerAlpha(char c) {
			return c >= 'a' && c <= 'z';
		}
		
		bool isUpperAlpha(char c) {
			return c >= 'A' && c <= 'Z';
		}
		
		bool isDigit(char c) {
			return c >= '0' && c <= '9';
		}
		
		bool isSpace(char c) {
			return c == ' ' || c == '\t' || c == '\n' || c == '\r';
		}
	}
}

#endif
