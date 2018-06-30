//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_EXAMPLE_FIELDS_ALLOC_HPP
#define BOOST_BEAST_EXAMPLE_FIELDS_ALLOC_HPP

#include <boost/throw_exception.hpp>
#include <cstdlib>
#include <memory>
#include <stdexcept>

namespace detail {

struct static_pool {
	std::size_t size_;
	std::size_t refs_ = 1;
	std::size_t count_ = 0;
	char* p_;
	char* end() {
		return reinterpret_cast<char*>(this + 1) + size_;
	}

	explicit static_pool(std::size_t size) :
			size_(size), p_(reinterpret_cast<char*>(this + 1)) {
	}

public:
	static static_pool&
	construct(std::size_t size) {
		auto p = new char[sizeof(static_pool) + size];
		return *(new (p) static_pool { size });
	}

	static_pool&
	share() {
		++refs_;
		return *this;
	}

	void destroy() {
		if (refs_--)
			return;
		this->~static_pool();
		delete[] reinterpret_cast<char*>(this);
	}

	void*
	alloc(std::size_t n) {
		auto last = p_ + n;
		if (last >= end())
			BOOST_THROW_EXCEPTION(std::bad_alloc { });
		++count_;
		auto p = p_;
		p_ = last;
		return p;
	}

	void dealloc() {
		if (--count_)
			return;
		p_ = reinterpret_cast<char*>(this + 1);
	}
};

} // detail

/** A non-thread-safe allocator optimized for @ref basic_fields.
 This allocator obtains memory from a pre-allocated memory block
 of a given size. It does nothing in deallocate until all
 previously allocated blocks are deallocated, upon which it
 resets the internal memory block for re-use.
 To use this allocator declare an instance persistent to the
 connection or session, and construct with the block size.
 A good rule of thumb is 20% more than the maximum allowed
 header size. For example if the application only allows up
 to an 8,000 byte header, the block size could be 9,600.
 Then, for every instance of `message` construct the header
 with a copy of the previously declared allocator instance.
 */
template<class T>
struct fields_alloc {
	detail::static_pool& pool_;

public:
	using value_type = T;
	using is_always_equal = std::false_type;
	using pointer = T*;
	using reference = T&;
	using const_pointer = T const*;
	using const_reference = T const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	template<class U>
	struct rebind {
		using other = fields_alloc<U>;
	};

	explicit fields_alloc(std::size_t size) :
			pool_(detail::static_pool::construct(size)) {
	}

	fields_alloc(fields_alloc const& other) :
			pool_(other.pool_.share()) {
	}

	template<class U>
	fields_alloc(fields_alloc<U> const& other) :
			pool_(other.pool_.share()) {
	}

	~fields_alloc() {
		pool_.destroy();
	}

	value_type*
	allocate(size_type n) {
		return static_cast<value_type*>(pool_.alloc(n * sizeof(T)));
	}

	void deallocate(value_type*, size_type) {
		pool_.dealloc();
	}

#if defined(BOOST_LIBSTDCXX_VERSION) && BOOST_LIBSTDCXX_VERSION < 60000
	template<class U, class... Args>
	void
	construct(U* ptr, Args&&... args)
	{
		::new((void*)ptr) U(std::forward<Args>(args)...);
	}

	template<class U>
	void
	destroy(U* ptr)
	{
		ptr->~U();
	}
#endif

	template<class U>
	friend
	bool operator==(fields_alloc const& lhs, fields_alloc<U> const& rhs) {
		return &lhs.pool_ == &rhs.pool_;
	}

	template<class U>
	friend
	bool operator!=(fields_alloc const& lhs, fields_alloc<U> const& rhs) {
		return !(lhs == rhs);
	}
};

#endif


//--------------------------------------------
//--------------------------------------------

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

// Return a reasonable mime type based on the extension of a file.
boost::string_view Util::mime_type(boost::string_view path) {
	auto const ext = [&path]
	{
		auto const pos = path.rfind(".");
		if(pos == boost::string_view::npos)
		return boost::string_view {};
		return path.substr(pos);
	}();
	if (boost::beast::iequals(ext, ".htm"))
		return "text/html";
	if (boost::beast::iequals(ext, ".html"))
		return "text/html";
	if (boost::beast::iequals(ext, ".php"))
		return "text/html";
	if (boost::beast::iequals(ext, ".css"))
		return "text/css";
	if (boost::beast::iequals(ext, ".txt"))
		return "text/plain";
	if (boost::beast::iequals(ext, ".js"))
		return "application/javascript";
	if (boost::beast::iequals(ext, ".json"))
		return "application/json";
	if (boost::beast::iequals(ext, ".xml"))
		return "application/xml";
	if (boost::beast::iequals(ext, ".swf"))
		return "application/x-shockwave-flash";
	if (boost::beast::iequals(ext, ".flv"))
		return "video/x-flv";
	if (boost::beast::iequals(ext, ".png"))
		return "image/png";
	if (boost::beast::iequals(ext, ".jpe"))
		return "image/jpeg";
	if (boost::beast::iequals(ext, ".jpeg"))
		return "image/jpeg";
	if (boost::beast::iequals(ext, ".jpg"))
		return "image/jpeg";
	if (boost::beast::iequals(ext, ".gif"))
		return "image/gif";
	if (boost::beast::iequals(ext, ".bmp"))
		return "image/bmp";
	if (boost::beast::iequals(ext, ".ico"))
		return "image/vnd.microsoft.icon";
	if (boost::beast::iequals(ext, ".tiff"))
		return "image/tiff";
	if (boost::beast::iequals(ext, ".tif"))
		return "image/tiff";
	if (boost::beast::iequals(ext, ".svg"))
		return "image/svg+xml";
	if (boost::beast::iequals(ext, ".svgz"))
		return "image/svg+xml";
	return "application/text";
}

}
#endif


//--------------------------------------------
//--------------------------------------------

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


//--------------------------------------------
//--------------------------------------------

/*
 * ExpressWeb
 * By: Brad Lee
 */
#ifndef EW_ERROR_CODE_H
#define EW_ERROR_CODE_H

namespace ExpressWeb {

class Error_code {
public:

};

}
#endif


//--------------------------------------------
//--------------------------------------------

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
public:
	Worker(Worker const&) = delete;
	Worker& operator=(Worker const&) = delete;

	Worker() {
	}

	Worker(boost::asio::ip::tcp::acceptor& acceptor) {
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

};

/* http implementation */
void Worker::setWriteCallback(std::function<void(response_t&)>& writeCallback) {
	writeCallback_ = writeCallback;
}
void Worker::setDocumentRoot(std::string& documentRoot) {
	documentRoot_ = documentRoot;
}
void Worker::setDocumentRoot(boost::string_view& documentRoot) {
	documentRoot_ = documentRoot;
}

void Worker::start() {
	accept();
	check_deadline();
}

void Worker::accept() {
	if (socket_ && acceptor_ && request_deadline_) {
		// Clean up any previous connection.
		boost::beast::error_code ec;
		socket_->close(ec);
		buffer_.consume(buffer_.size());

		acceptor_.get().async_accept(socket_.get(),
				[this](boost::beast::error_code ec)
				{
					if (ec)
					{
						accept();
					}
					else
					{
						// Request must be fully processed within 60 seconds.
						request_deadline_->expires_after(
								std::chrono::seconds(60));

						read_request();
					}
				});
	}
}

void Worker::read_request() {
	// On each read the parser needs to be destroyed and
	// recreated. We store it in a boost::optional to
	// achieve that.
	//
	// Arguments passed to the parser constructor are
	// forwarded to the message object. A single argument
	// is forwarded to the body constructor.
	//
	// We construct the dynamic body with a 1MB limit
	// to prevent vulnerability to buffer attacks.
	//
	parser_.emplace(std::piecewise_construct, std::make_tuple(),
			std::make_tuple(alloc_));

	if (socket_) {
		boost::beast::http::async_read(socket_.get(), buffer_, parser_.get(),
				[this](boost::beast::error_code ec, std::size_t)
				{
					if (ec)
					accept();
					else
					process_request(parser_->get());
				});
	}
}

void Worker::process_request(request_t const& req) {
	switch (req.method()) {
	case boost::beast::http::verb::get:
		send_file(req.target());
		break;

	default:
		// We return responses indicating an error if
		// we do not recognize the request method.
		send_bad_response(boost::beast::http::status::bad_request,
				"Invalid request-method '" + req.method_string().to_string()
						+ "'\r\n");
		break;
	}
}

void Worker::send_bad_response(boost::beast::http::status status,
		std::string const& error) {
	string_response_.emplace(std::piecewise_construct, std::make_tuple(),
			std::make_tuple(alloc_));

	string_response_->result(status);
	string_response_->keep_alive(false);
	string_response_->set(boost::beast::http::field::server, "Beast");
	string_response_->set(boost::beast::http::field::content_type,
			"text/plain");
	string_response_->body() = error;
	string_response_->prepare_payload();

	if (writeCallback_) {
		// write callback
		writeCallback_.get()(string_response_.get());
	}
	if (socket_) {
		string_serializer_.emplace(string_response_.get());
		boost::beast::http::async_write(socket_.get(), string_serializer_.get(),
				[this](boost::beast::error_code ec, std::size_t)
				{
					socket_.get().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
					string_serializer_.reset();
					string_response_.reset();
					accept();
				});
	}
}

void Worker::send_file(boost::string_view target) {
	// Request path must be absolute and not contain "..".
	if (target.empty() || target[0] != '/'
			|| target.find("..") != std::string::npos) {
		send_bad_response(boost::beast::http::status::not_found,
				"File not found\r\n");
		return;
	}
	std::string targetBase;
	size_t queryDelimeterPos;
	if ((queryDelimeterPos = target.find("?")) != std::string::npos) {
		targetBase.append(target.data(), queryDelimeterPos);
	} else {
		targetBase.append(target.data(), target.size());
	}
	if (boost::algorithm::ends_with(targetBase, "/")) {
		targetBase.append("index.html");
	}

	std::string full_path;
	if (documentRoot_) {
		full_path.append(documentRoot_.get().to_string());
		full_path.append(targetBase);
	}

	if (writeCallback_) {
		// write callback
		std::ifstream file;
		file.open(full_path.c_str());
		if (!file.is_open()) {
			std::string errorMsg;
			errorMsg.append("File not found\r\n").append("fullpath: ").append(
					full_path).append("\nbase: ").append(targetBase).append(
					"\n");
			send_bad_response(boost::beast::http::status::not_found, errorMsg);
			return;
		}
		string_response_.emplace(std::piecewise_construct, std::make_tuple(),
				std::make_tuple(alloc_));

		string_response_->result(boost::beast::http::status::ok);
		string_response_->keep_alive(false);
		string_response_->set(boost::beast::http::field::server, "Beast");
		string_response_->set(boost::beast::http::field::content_type,
				ExpressWeb::Util::mime_type(targetBase));

		file.seekg(0, std::ios::end);
		string_response_->body().reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		string_response_->body().assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

		string_response_->prepare_payload();

		writeCallback_.get()(string_response_.get());
	}
	if (socket_) {
		// write to socket
		boost::beast::http::file_body::value_type file;
		boost::beast::error_code ec;
		file.open(full_path.c_str(), boost::beast::file_mode::read, ec);
		if (ec) {
			send_bad_response(boost::beast::http::status::not_found,
					"File not found\r\n");
			return;
		}

		file_response_.emplace(std::piecewise_construct, std::make_tuple(),
				std::make_tuple(alloc_));

		file_response_->result(boost::beast::http::status::ok);
		file_response_->keep_alive(false);
		file_response_->set(boost::beast::http::field::server, "Beast");
		file_response_->set(boost::beast::http::field::content_type,
				ExpressWeb::Util::mime_type(targetBase));
		file_response_->body() = std::move(file);
		file_response_->prepare_payload();

		file_serializer_.emplace(file_response_.get());
		boost::beast::http::async_write(socket_.get(), file_serializer_.get(),
				[this](boost::beast::error_code ec, std::size_t)
				{
					socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
					file_serializer_.reset();
					file_response_.reset();
					accept();
				});
	}
}

void Worker::check_deadline() {
	if (request_deadline_) {
		// The deadline may have moved, so check it has really passed.
		if (request_deadline_->expiry() <= std::chrono::steady_clock::now()) {
			// Close socket to cancel any outstanding operation.
			boost::beast::error_code ec;
			socket_->close();

			// Sleep indefinitely until we're given a new deadline.
			request_deadline_->expires_at(
					std::chrono::steady_clock::time_point::max());
		}

		request_deadline_->async_wait([this](boost::beast::error_code)
		{
			check_deadline();
		});
	}
}

} // namespace http

} // namespace ExpressWeb


//--------------------------------------------
//--------------------------------------------

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


//--------------------------------------------
//--------------------------------------------

