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
	operator bool() const {
		return true;
	}
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
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <functional>







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


//--------------------------------------------
//--------------------------------------------

