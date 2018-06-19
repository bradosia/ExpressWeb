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

// Return a reasonable mime type based on the extension of a file.
boost::beast::string_view mime_type(boost::beast::string_view path) {
	using boost::beast::iequals;
	auto const ext = [&path]
	{
		auto const pos = path.rfind(".");
		if(pos == boost::beast::string_view::npos)
		return boost::beast::string_view {};
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

/*char* stripslashes(const char* const &str) {
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
 }
 i += 2;
 }
 answer[k] = 0;
 return answer;
 }*/

std::string stripslashes(std::string const &str) {
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

class http_worker {
private:
	// The acceptor used to listen for incoming connections.
	boost::optional<tcp::acceptor> acceptor_;

	// The path to the root of the document directory.
	boost::optional<boost::string_view> documentRoot_;

	// The socket for the currently connected client.
	boost::optional<tcp::socket> socket_;

	// The buffer for performing reads
	boost::beast::flat_static_buffer<8192> buffer_;

	// The allocator used for the fields in the request and reply.
	alloc_t alloc_ { 8192 };

	// The parser for reading the requests
	boost::optional<http::request_parser<request_body_t, alloc_t>> parser_;

	// The timer putting a time limit on requests.
	boost::optional<waitable_timer_t> request_deadline_;

	// The string-based response message.
	boost::optional<response_t> string_response_;

	// The string-based response serializer.
	boost::optional<
			http::response_serializer<http::string_body,
					http::basic_fields<alloc_t>>> string_serializer_;

	// The file-based response message.
	boost::optional<response_file_t> file_response_;

	// The file-based response serializer.
	boost::optional<
			http::response_serializer<http::file_body,
					http::basic_fields<alloc_t>>> file_serializer_;

	// send callback
	boost::optional<std::function<void(response_t&)>> writeCallback_;
	boost::optional<std::function<void(response_file_t&)>> writeFileCallback_;
public:
	http_worker(http_worker const&) = delete;
	http_worker& operator=(http_worker const&) = delete;

	http_worker(std::function<void(response_t&)>& writeCallback,
			std::function<void(response_file_t&)>& writeFileCallback) {
		writeCallback_ = writeCallback;
		writeFileCallback_ = writeFileCallback;
	}

	/*http_worker(tcp::acceptor& acceptor, const std::string& doc_root) :
	 acceptor_(acceptor), documentRoot_(doc_root), socket_(
	 tcp::socket(acceptor.get_executor().context())), request_deadline_(
	 waitable_timer_t(acceptor.get_executor().context(),
	 (std::chrono::steady_clock::time_point::max)())) {
	 }*/

	void setDocumentRoot(std::string& documentRoot) {
		documentRoot_ = documentRoot;
	}
	void setDocumentRoot(boost::string_view& documentRoot) {
		documentRoot_ = documentRoot;
	}

	void start() {
		accept();
		check_deadline();
	}

	void accept() {
		// Clean up any previous connection.
		boost::beast::error_code ec;
		socket_.get().close(ec);
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

	void read_request() {
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

		http::async_read(socket_.get(), buffer_, parser_.get(),
				[this](boost::beast::error_code ec, std::size_t)
				{
					if (ec)
					accept();
					else
					process_request(parser_->get());
				});
	}

	void process_request(
			http::request<request_body_t, http::basic_fields<alloc_t>> const& req) {
		switch (req.method()) {
		case http::verb::get:
			send_file(req.target());
			break;

		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			send_bad_response(http::status::bad_request,
					"Invalid request-method '" + req.method_string().to_string()
							+ "'\r\n");
			break;
		}
	}

	void send_bad_response(http::status status, std::string const& error) {
		string_response_.emplace(std::piecewise_construct, std::make_tuple(),
				std::make_tuple(alloc_));

		string_response_->result(status);
		string_response_->keep_alive(false);
		string_response_->set(http::field::server, "Beast");
		string_response_->set(http::field::content_type, "text/plain");
		string_response_->body() = error;
		string_response_->prepare_payload();

		if (writeCallback_) {
			// custom write callback
			writeCallback_.get()(string_response_.get());
		} else if (socket_) {
			string_serializer_.emplace(string_response_.get());
			http::async_write(socket_.get(), string_serializer_.get(),
					[this](boost::beast::error_code ec, std::size_t)
					{
						socket_.get().shutdown(tcp::socket::shutdown_send, ec);
						string_serializer_.reset();
						string_response_.reset();
						accept();
					});
		}
	}

	void send_file(boost::beast::string_view target) {
		// Request path must be absolute and not contain "..".
		if (target.empty() || target[0] != '/'
				|| target.find("..") != std::string::npos) {
			send_bad_response(http::status::not_found, "File not found\r\n");
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

		if (writeFileCallback_) {
			// custom write callback
			std::ifstream file;
			std::string full_path;
			if (documentRoot_) {
				full_path.append(documentRoot_.get().data(),
						documentRoot_.get().size());
				full_path.append(targetBase);
				file.open(full_path.c_str());
			}
			if (!file.is_open()) {
				std::string errorMsg;
				errorMsg.append("File not found\r\n").append("fullpath: ").append(
						full_path).append("\nbase: ").append(targetBase).append(
						"\n");
				send_bad_response(http::status::not_found, errorMsg);
				return;
			}
			string_response_.emplace(std::piecewise_construct,
					std::make_tuple(), std::make_tuple(alloc_));

			string_response_->result(http::status::ok);
			string_response_->keep_alive(false);
			string_response_->set(http::field::server, "Beast");
			string_response_->set(http::field::content_type,
					mime_type(targetBase));

			file.seekg(0, std::ios::end);
			string_response_->body().reserve(file.tellg());
			file.seekg(0, std::ios::beg);
			string_response_->body().assign(
					(std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());

			string_response_->prepare_payload();

			writeCallback_.get()(string_response_.get());
		} else if (socket_) {
			// write to socket
			http::file_body::value_type file;
			boost::beast::error_code ec;
			std::string full_path;
			if (documentRoot_) {
				full_path = documentRoot_.get().data();
				full_path.append(targetBase);
				file.open(full_path.c_str(), boost::beast::file_mode::read, ec);
			}
			if (ec) {
				send_bad_response(http::status::not_found,
						"File not found\r\n");
				return;
			}

			file_response_.emplace(std::piecewise_construct, std::make_tuple(),
					std::make_tuple(alloc_));

			file_response_->result(http::status::ok);
			file_response_->keep_alive(false);
			file_response_->set(http::field::server, "Beast");
			file_response_->set(http::field::content_type,
					mime_type(targetBase));
			file_response_->body() = std::move(file);
			file_response_->prepare_payload();

			file_serializer_.emplace(file_response_.get());
			http::async_write(socket_.get(), file_serializer_.get(),
					[this](boost::beast::error_code ec, std::size_t)
					{
						socket_->shutdown(tcp::socket::shutdown_send, ec);
						file_serializer_.reset();
						file_response_.reset();
						accept();
					});
		}
	}

	void check_deadline() {
		// The deadline may have moved, so check it has really passed.
		if (request_deadline_.get().expiry()
				<= std::chrono::steady_clock::now()) {
			// Close socket to cancel any outstanding operation.
			boost::beast::error_code ec;
			socket_->close();

			// Sleep indefinitely until we're given a new deadline.
			request_deadline_.get().expires_at(
					std::chrono::steady_clock::time_point::max());
		}

		request_deadline_.get().async_wait([this](boost::beast::error_code)
		{
			check_deadline();
		});
	}
};

class myException: public std::exception {
public:
	const char* msg_;
	myException(const char* msg) :
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
			throw myException(const_cast<const char*>(errorMsg.c_str()));
		} else if (!jsonDocument.IsObject()) {
			throw myException("JSON is not an object");
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
						stripslashes(
								jsonDocumentObject["REQUEST_URI"].GetString()));
			}
			for (auto& m : jsonDocument.GetObject()) {
				if (m.name.IsString() && m.value.IsString()) {
					req.set(m.name.GetString(),
							stripslashes(m.value.GetString()));
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
		// UNUSED - FILE CALLBACK
		std::function<void(response_file_t&)> writeFileCallback =
				[](response_file_t& res) {
					for (auto ptr = res.begin(); ptr != res.end(); ptr++) {
						std::cout << ptr->name() << ": " << ptr->value() << "\n";
					}
				};

		http_worker worker(writeCallback, writeFileCallback);
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
