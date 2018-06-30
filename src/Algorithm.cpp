#include "Algorithm.h"

namespace ExpressWeb {

char* Algorithm::stripslashesC(const char* const &str) {
	char* answer = new char[std::strlen(str)];
	unsigned int i = 0;
	unsigned int k = 0;
	while (i < std::strlen(str) - 1) {
		if (str[i] != '\\') {
			answer[k++] = str[i++];
		} else {
			switch (str[i + 1]) {
			case '\'':
				answer[k++] = '\'';
				break;
			case '\"':
				answer[k++] = '\"';
				break;
			case '\\':
				answer[k++] = '\\';
				break;
			case '\0':
				answer[k++] = '\0';
				break;
			case '/':
				answer[k++] = '/';
				break;
			}
			i += 2;
		}
	}
	answer[k] = 0;
	return answer;
}

std::string Algorithm::stripslashes(std::string const &str) {
	std::string answer;
	unsigned int i = 0;
	while (i < str.size()) {
		if (str[i] != '\\') {
			answer.push_back(str[i++]);
		} else {
			switch (str[i + 1]) {
			case '\'':
				answer.push_back('\'');
				break;
			case '\"':
				answer.push_back('\"');
				break;
			case '\\':
				answer.push_back('\\');
				break;
			case '\0':
				answer.push_back('\0');
				break;
			case '/':
				answer.push_back('/');
				break;
			}
			i += 2;
		}
	}
	return answer;
}

}
