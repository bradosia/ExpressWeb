//
// Copyright (c) 2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, fast
//
//------------------------------------------------------------------------------
#ifndef EW_SERVER_H
#define EW_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <functional>
#include "fields_alloc.h"

#include "Error_code.h"
#include "Http.h"
#include "Algorithm.h"
#include "Util.h"

namespace ExpressWeb {

class Server {
public:
	using callBackClientError_t = std::function<void(ExpressWeb::Error_code, boost::asio::ip::tcp::socket)>;
	using callBackClose_t = std::function<void()>;
	using callBackConnection_t = std::function<void(boost::asio::ip::tcp::socket)>;
	using callBackRequest_t = std::function<void(boost::asio::ip::tcp::socket)>;
private:
	boost::optional<unsigned short> port_;
	boost::optional<boost::asio::ip::address> address_;
	boost::optional<bool> poll_;
	boost::optional<unsigned int> workerNum_;
	boost::optional<boost::string_view> documentRoot_;
	bool staticServe_;
	boost::optional<callBackClientError_t> callBackClientError;
	boost::optional<callBackClose_t> callBackClose;
	boost::optional<callBackConnection_t> callBackConnection;
	boost::optional<callBackRequest_t> callBackRequest;
public:
	Server() :
			staticServe_(false) {

	}
	/* single set */
	void setPort(boost::optional<int>& port);
	void setPort(int port);
	void setPort(unsigned int port);
	void setPort(unsigned short port);
	void setAddress(boost::optional<boost::string_view>& address);
	void setAddress(const char* const address);
	void setAddress(const std::string& address);
	void setAddress(const boost::string_view& address);
	void setAddress(boost::asio::ip::address address);
	void setPoll(boost::optional<bool>& poll);
	void setPoll(bool poll);
	void setWorkerNum(boost::optional<int>& workerNum);
	void setWorkerNum(int workerNum);
	void setWorkerNum(unsigned int workerNum);
	void setDocumentRoot(boost::optional<boost::string_view>& documentRoot);
	void setDocumentRoot(const char* const documentRoot);
	void setDocumentRoot(const std::string& documentRoot);
	void setDocumentRoot(const boost::string_view& documentRoot);
	/** If static serve is set to true,
	 * The server will use the document root to serve static files
	 */
	void setStaticServe(bool staticServe);
	/* main interface */
	void connection(unsigned short port);
	void onClientError(callBackClientError_t cb);
	void onClose(callBackClose_t cb);
	void onConnection(callBackConnection_t cb);
	void onRequest(callBackRequest_t cb);
	void start();
	void start(callBackClientError_t cb);
	class infoObject {
	public:
		std::string uri;
	};
	infoObject info;
};

} // namespace ExpressWeb
#endif
