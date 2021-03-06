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

#include "fields_alloc.hpp"

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

#ifndef ONE_HEADER
#include <rapidjson.h>
#endif

namespace ip = boost::asio::ip;
// from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;
// from <boost/asio.hpp>
namespace http = boost::beast::http;
// from <boost/beast/http.hpp>

using headerMap_t = std::unordered_map<std::string, boost::string_view>;
using request_body_t = http::string_body;
//using request_body_t = http::basic_dynamic_body<boost::beast::flat_static_buffer<1024 * 1024>>;
using alloc_t = fields_alloc<char>;
using waitable_timer_t = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;
using request_t = http::request<http::string_body, http::basic_fields<alloc_t>>;
using response_t = http::response<http::string_body, http::basic_fields<alloc_t>>;
using response_file_t = http::response<http::file_body, http::basic_fields<alloc_t>>;

namespace ExpressWeb {

class Algorithm {
public:
	static char* stripslashesC(const char* const &str);
	static std::string stripslashes(std::string const &str);
};

class Util {
public:
	static boost::string_view mime_type(boost::string_view path);
};

char* Algorithm::stripslashesC(const char* const &str) {
	char* answer = new char[strlen(str)];
	unsigned int i = 0;
	unsigned int k = 0;
	while (i < strlen(str) - 1) {
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

// Return a reasonable mime type based on the extension of a file.
boost::string_view Util::mime_type(boost::string_view path) {
	using boost::beast::iequals;
	auto const ext = [&path]
	{
		auto const pos = path.rfind(".");
		if(pos == boost::string_view::npos)
		return boost::string_view {};
		return path.substr(pos);
	}();
	if (iequals(ext, ".htm"))
		return "text/html";
	if (iequals(ext, ".html"))
		return "text/html";
	if (iequals(ext, ".php"))
		return "text/html";
	if (iequals(ext, ".css"))
		return "text/css";
	if (iequals(ext, ".txt"))
		return "text/plain";
	if (iequals(ext, ".js"))
		return "application/javascript";
	if (iequals(ext, ".json"))
		return "application/json";
	if (iequals(ext, ".xml"))
		return "application/xml";
	if (iequals(ext, ".swf"))
		return "application/x-shockwave-flash";
	if (iequals(ext, ".flv"))
		return "video/x-flv";
	if (iequals(ext, ".png"))
		return "image/png";
	if (iequals(ext, ".jpe"))
		return "image/jpeg";
	if (iequals(ext, ".jpeg"))
		return "image/jpeg";
	if (iequals(ext, ".jpg"))
		return "image/jpeg";
	if (iequals(ext, ".gif"))
		return "image/gif";
	if (iequals(ext, ".bmp"))
		return "image/bmp";
	if (iequals(ext, ".ico"))
		return "image/vnd.microsoft.icon";
	if (iequals(ext, ".tiff"))
		return "image/tiff";
	if (iequals(ext, ".tif"))
		return "image/tiff";
	if (iequals(ext, ".svg"))
		return "image/svg+xml";
	if (iequals(ext, ".svgz"))
		return "image/svg+xml";
	return "application/text";
}

namespace http {
class Worker {
private:
	// The acceptor used to listen for incoming connections.
	boost::optional<tcp::acceptor&> acceptor_;

	// The path to the root of the document directory.
	boost::optional<boost::string_view> documentRoot_;

	// The socket for the currently connected client.
	boost::optional<tcp::socket> socket_;

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

	Worker(tcp::acceptor& acceptor) {
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
					socket_.get().shutdown(tcp::socket::shutdown_send, ec);
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
					socket_->shutdown(tcp::socket::shutdown_send, ec);
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

class App {
private:
	boost::optional<unsigned short> port_;
	boost::optional<boost::asio::ip::address> address_;
	boost::optional<bool> poll_;
	boost::optional<unsigned int> workerNum_;
	boost::optional<boost::string_view> documentRoot_;
public:
	App() {

	}
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
	void start();
};

/*
 * App implementation
 */

void App::setPort(boost::optional<int>& port) {
	if (port) {
		setPort(port.get());
	}
}
void App::setPort(int port) {
	port_ = static_cast<unsigned short>(port);
}
void App::setPort(unsigned int port) {
	port_ = static_cast<unsigned short>(port);
}
void App::setPort(unsigned short port) {
	port_ = port;
}
void App::setAddress(boost::optional<boost::string_view>& address) {
	if (address) {
		setAddress(address.get());
	}
}
void App::setAddress(const char* const address) {
	boost::system::error_code ec;
	address_ = boost::asio::ip::make_address(address, ec);
}
void App::setAddress(const std::string& address) {
	boost::system::error_code ec;
	address_ = boost::asio::ip::make_address(address, ec);
}
void App::setAddress(const boost::string_view& address) {
	setAddress(address.to_string());
}
void App::setAddress(boost::asio::ip::address address) {
	address_ = address;
}
void App::setPoll(boost::optional<bool>& poll) {
	if (poll) {
		setPoll(poll.get());
	}
}
void App::setPoll(bool poll) {
	poll_ = poll;
}
void App::setWorkerNum(boost::optional<int>& workerNum) {
	if (workerNum) {
		setWorkerNum(workerNum.get());
	}
}
void App::setWorkerNum(int workerNum) {
	workerNum_ = static_cast<unsigned int>(workerNum);
}
void App::setWorkerNum(unsigned int workerNum) {
	workerNum_ = workerNum;
}
void App::setDocumentRoot(boost::optional<boost::string_view>& documentRoot) {
	if (documentRoot) {
		setDocumentRoot(documentRoot.get());
	}
}
void App::setDocumentRoot(const char* const documentRoot) {
	documentRoot_.emplace(documentRoot);
}
void App::setDocumentRoot(const std::string& documentRoot) {
	documentRoot_.emplace(documentRoot);
}
void App::setDocumentRoot(const boost::string_view& documentRoot) {
	documentRoot_.emplace(documentRoot);
}
void App::start() {
	boost::asio::io_context ioc { 1 };
	tcp::acceptor acceptor { ioc, { address_.get(), port_.get() } };

	unsigned int i, n;
	n = workerNum_.get();
	std::list<ExpressWeb::http::Worker> workers;
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

class MyException: public std::exception {
public:
	const char* msg_;
	MyException(const char* msg) :
			msg_(msg) {
	}
	virtual const char* what() const throw () {
		return msg_;
	}
};

int main(int argc, char* argv[]) {
	std::string temp;
	try {
		boost::string_view HTTP_request;
		boost::optional < boost::string_view > serverAddress;
		boost::optional<int> serverPort;
		boost::optional < boost::string_view > documentRoot;
		boost::optional<int> workerThreadsNum;
		boost::optional<bool> serverPoll;
		boost::optional < boost::string_view > indexPage;
		boost::optional<headerMap_t> clientHeaders;
		if (argc > 1) {
			HTTP_request = argv[1];
		}

		rapidjson::Document jsonDocument; // Document is GenericDocument<UTF8<> >
		jsonDocument.Parse(HTTP_request.data());
		alloc_t alloc_ { 8192 };
		http::request<request_body_t, http::basic_fields<alloc_t>> req(
				std::piecewise_construct, std::make_tuple(),
				std::make_tuple(alloc_));

		if (jsonDocument.HasParseError()) {
			std::string errorMsg;
			errorMsg.append("Failed to parse JSON!");
			if (jsonDocument.GetParseError() == 3) {
				errorMsg.append(" Possible reason: file encoding incorrect\n");
			} else {
				errorMsg.append(" Error code: ").append(
						std::to_string(jsonDocument.GetParseError())).append(
						" at position ").append(
						std::to_string(jsonDocument.GetErrorOffset()));
			}
			throw MyException(const_cast<const char*>(errorMsg.c_str()));
		} else if (!jsonDocument.IsObject()) {
			throw MyException("JSON is not an object");
		} else {
			auto jsonDocumentObject = jsonDocument.GetObject();
			if (jsonDocumentObject.HasMember("REQUEST_METHOD")
					&& jsonDocumentObject["REQUEST_METHOD"].IsString()) {
				std::string&& verb =
						jsonDocumentObject["REQUEST_METHOD"].GetString();
				if (verb == "GET") {
					req.method(http::verb::get);
				}
			}
			if (jsonDocumentObject.HasMember("REQUEST_URI")
					&& jsonDocumentObject["REQUEST_URI"].IsString()) {
				req.target(
						ExpressWeb::Algorithm::stripslashes(
								jsonDocumentObject["REQUEST_URI"].GetString()));
			}
			for (auto& m : jsonDocument.GetObject()) {
				if (m.name.IsString() && m.value.IsString()) {
					req.set(m.name.GetString(),
							ExpressWeb::Algorithm::stripslashes(m.value.GetString()));
				} else if (m.name.IsString() && m.value.IsInt()) {
					req.set(m.name.GetString(),
							std::to_string(m.value.GetInt()));
				} else if (m.name.IsString() && m.value.IsFloat()) {
					req.set(m.name.GetString(),
							std::to_string(m.value.GetFloat()));
				}
			}
		}

		//std::cout << req << "\n";

		std::function<void(response_t&)> writeCallback = [](response_t& res) {
			std::cout << res << "\n";
		};

		ExpressWeb::http::Worker worker;
		worker.setWriteCallback(writeCallback);
		documentRoot = boost::algorithm::erase_all_copy(
				req.at("DOCUMENT_ROOT").to_string(), "\n");
		if (documentRoot) {
			worker.setDocumentRoot(documentRoot.get());
		}
		worker.process_request(req);

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
