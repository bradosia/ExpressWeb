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
#ifndef EW_HTTP_H
#define EW_HTTP_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>				// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <boost/algorithm/string/predicate.hpp>		// boost::algorithm::ends_with
#include <boost/algorithm/string/erase.hpp>			// boost::algorithm::erase_all_copy
#include <chrono>
#include <string>
#include "fields_alloc.h"

#include "Error_code.h"
#include "Algorithm.h"
#include "Util.h"

namespace ExpressWeb {

namespace Http {
class Worker {
public:
	using headerMap_t = std::unordered_map<std::string, boost::string_view>;
	using request_body_t = boost::beast::http::string_body;
	//using request_body_t = http::basic_dynamic_body<boost::beast::flat_static_buffer<1024 * 1024>>;
	using alloc_t = fields_alloc<char>;
	using waitable_timer_t = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;
	using request_t = boost::beast::http::request<boost::beast::http::string_body, boost::beast::http::basic_fields<alloc_t>>;
	using response_t = boost::beast::http::response<boost::beast::http::string_body, boost::beast::http::basic_fields<alloc_t>>;
	using response_file_t = boost::beast::http::response<boost::beast::http::file_body, boost::beast::http::basic_fields<alloc_t>>;

	using callBackClientError_t = std::function<void(ExpressWeb::Error_code, boost::asio::ip::tcp::socket)>;
	using callBackClose_t = std::function<void()>;
	using callBackConnection_t = std::function<void(boost::asio::ip::tcp::socket)>;
	using callBackRequest_t = std::function<void(boost::asio::ip::tcp::socket)>;
private:
	// The acceptor used to listen for incoming connections.
	boost::optional<boost::asio::ip::tcp::acceptor&> acceptor_;

	// The path to the root of the document directory.
	boost::optional<boost::string_view> documentRoot_;

	// The socket for the currently connected client.
	boost::optional<boost::asio::ip::tcp::socket> socket_;

	// The buffer for performing reads
	boost::beast::flat_static_buffer<8192> buffer_;

	// The allocator used for the fields in the request and reply.
	alloc_t alloc_ { 8192 };

	// The parser for reading the requests
	boost::optional<boost::beast::http::request_parser<request_body_t, alloc_t>> parser_;

	// The timer putting a time limit on requests.
	boost::optional<waitable_timer_t> request_deadline_;

	// The string-based response message.
	boost::optional<response_t> string_response_;

	// The string-based response serializer.
	boost::optional<
			boost::beast::http::response_serializer<
					boost::beast::http::string_body,
					boost::beast::http::basic_fields<alloc_t>>> string_serializer_;

	// The file-based response message.
	boost::optional<response_file_t> file_response_;

	// The file-based response serializer.
	boost::optional<
			boost::beast::http::response_serializer<
					boost::beast::http::file_body,
					boost::beast::http::basic_fields<alloc_t>>> file_serializer_;

	// send callback
	boost::optional<std::function<void(response_t&)>> writeCallback_;

	bool staticServe_;
	boost::optional<callBackClientError_t> callBackClientError;
	boost::optional<callBackClose_t> callBackClose;
	boost::optional<callBackConnection_t> callBackConnection;
	boost::optional<callBackRequest_t> callBackRequest;
public:
	Worker(Worker const&) = delete;
	Worker& operator=(Worker const&) = delete;

	Worker() :
			staticServe_(false) {
	}

	Worker(boost::asio::ip::tcp::acceptor& acceptor) :
			staticServe_(false) {
		acceptor_ = acceptor;
		socket_.emplace(acceptor.get_executor().context());
		request_deadline_.emplace(acceptor.get_executor().context(),
				(std::chrono::steady_clock::time_point::max)());
	}

	void setWriteCallback(std::function<void(response_t&)>& writeCallback);
	void setDocumentRoot(std::string& documentRoot);
	void setDocumentRoot(boost::string_view& documentRoot);
	void start();
	void accept();
	void read_request();
	void process_request(request_t const& req);
	void send_bad_response(boost::beast::http::status status,
			std::string const& error);
	void send_file(boost::string_view target);
	void check_deadline();
	/** If static serve is set to true,
	 * The server will use the document root to serve static files
	 */
	void setStaticServe(bool staticServe);
	/* main interface */
	void onClientError(callBackClientError_t cb);
	void onClose(callBackClose_t cb);
	void onConnection(callBackConnection_t cb);
	void onRequest(callBackRequest_t cb);
};

} // namespace http

} // namespace ExpressWeb
#endif
