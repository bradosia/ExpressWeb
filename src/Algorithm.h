/*
 * ExpressWeb
 * By: Brad Lee
 */
#ifndef EW_ALGORITHM_H
#define EW_ALGORITHM_H

#include <cstring>			// std::strlen
#include <string>			// std::string

namespace ExpressWeb {

class Algorithm {
public:
	static char* stripslashesC(const char* const &str);
	static std::string stripslashes(std::string const &str);
};

}

#endif
