/*
 * ExpressWeb Server
 * By: Brad Lee
 */
#ifndef EW_UTIL_H
#define EW_UTIL_H

#include <boost/utility/string_view.hpp>
#include <boost/beast/core.hpp>

namespace ExpressWeb {

class Util {
public:
	static boost::string_view mime_type(boost::string_view path);
};

}

#endif
