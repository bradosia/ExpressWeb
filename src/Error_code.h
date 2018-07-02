/*
 * ExpressWeb
 * By: Brad Lee
 */
#ifndef EW_ERROR_CODE_H
#define EW_ERROR_CODE_H

namespace ExpressWeb {

class Error_code {
public:
	operator bool() const {
		return true;
	}
};

}
#endif
