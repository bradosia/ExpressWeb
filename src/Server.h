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
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <boost/algorithm/string/predicate.hpp>		// boost::algorithm::ends_with
#include <boost/algorithm/string/erase.hpp>			// boost::algorithm::erase_all_copy
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include "fields_alloc.h"

#include "Http.h"
#include "Algorithm.h"
#include "Util.h"

namespace ExpressWeb {

class Server {
private:
	boost::optional<unsigned short> port_;
	boost::optional<boost::asio::ip::address> address_;
	boost::optional<bool> poll_;
	boost::optional<unsigned int> workerNum_;
	boost::optional<boost::string_view> documentRoot_;
public:
	Server() {

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
	/* main interface */
	void connection();
	void start();
	class infoObject {
	public:
		std::string uri;
	};
	infoObject info;
};

/*
 * Server implementation
 */

void Server::setPort(boost::optional<int>& port) {
	if (port) {
		setPort(port.get());
	}
}
void Server::setPort(int port) {
	port_ = static_cast<unsigned short>(port);
}
void Server::setPort(unsigned int port) {
	port_ = static_cast<unsigned short>(port);
}
void Server::setPort(unsigned short port) {
	port_ = port;
}
void Server::setAddress(boost::optional<boost::string_view>& address) {
	if (address) {
		setAddress(address.get());
	}
}
void Server::setAddress(const char* const address) {
	boost::system::error_code ec;
	address_ = boost::asio::ip::make_address(address, ec);
}
void Server::setAddress(const std::string& address) {
	boost::system::error_code ec;
	address_ = boost::asio::ip::make_address(address, ec);
}
void Server::setAddress(const boost::string_view& address) {
	setAddress(address.to_string());
}
void Server::setAddress(boost::asio::ip::address address) {
	address_ = address;
}
void Server::setPoll(boost::optional<bool>& poll) {
	if (poll) {
		setPoll(poll.get());
	}
}
void Server::setPoll(bool poll) {
	poll_ = poll;
}
void Server::setWorkerNum(boost::optional<int>& workerNum) {
	if (workerNum) {
		setWorkerNum(workerNum.get());
	}
}
void Server::setWorkerNum(int workerNum) {
	workerNum_ = static_cast<unsigned int>(workerNum);
}
void Server::setWorkerNum(unsigned int workerNum) {
	workerNum_ = workerNum;
}
void Server::setDocumentRoot(
		boost::optional<boost::string_view>& documentRoot) {
	if (documentRoot) {
		setDocumentRoot(documentRoot.get());
	}
}
void Server::setDocumentRoot(const char* const documentRoot) {
	documentRoot_.emplace(documentRoot);
}
void Server::setDocumentRoot(const std::string& documentRoot) {
	documentRoot_.emplace(documentRoot);
}
void Server::setDocumentRoot(const boost::string_view& documentRoot) {
	documentRoot_.emplace(documentRoot);
}

void Server::connection() {

}

void Server::start() {
	boost::asio::io_context ioc { 1 };
	boost::asio::ip::tcp::acceptor acceptor { ioc,
			{ address_.get(), port_.get() } };

	unsigned int i, n;
	n = workerNum_.get();
	std::list<ExpressWeb::Http::Worker> workers;
	for (i = 0; i < n; ++i) {
		workers.emplace_back(acceptor);
		workers.back().setDocumentRoot(documentRoot_.get());
		workers.back().start();
	}

	if (poll_.get())
		for (;;)
			ioc.poll();
	else
		ioc.run();
}

} // namespace ExpressWeb
#endif
