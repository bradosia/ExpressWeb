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

class http_worker {
public:
	http_worker(http_worker const&) = delete;
	http_worker& operator=(http_worker const&) = delete;

	http_worker(tcp::acceptor& acceptor, const std::string& doc_root) :
			acceptor_(acceptor), doc_root_(doc_root) {
	}

	void start() {
		accept();
		check_deadline();
	}

private:
	using alloc_t = fields_alloc<char>;
	using request_body_t = http::basic_dynamic_body<boost::beast::flat_static_buffer<1024 * 1024>>;

	// The acceptor used to listen for incoming connections.
	tcp::acceptor& acceptor_;

	// The path to the root of the document directory.
	std::string doc_root_;

	// The socket for the currently connected client.
	tcp::socket socket_ { acceptor_.get_executor().context() };

	// The buffer for performing reads
	boost::beast::flat_static_buffer<8192> buffer_;

	// The allocator used for the fields in the request and reply.
	alloc_t alloc_ { 8192 };

	// The parser for reading the requests
	boost::optional<http::request_parser<request_body_t, alloc_t>> parser_;

	// The timer putting a time limit on requests.
	boost::asio::basic_waitable_timer<std::chrono::steady_clock> request_deadline_ {
			acceptor_.get_executor().context(),
			(std::chrono::steady_clock::time_point::max)() };

	// The string-based response message.
	boost::optional<
			http::response<http::string_body, http::basic_fields<alloc_t>>> string_response_;

	// The string-based response serializer.
	boost::optional<
			http::response_serializer<http::string_body,
					http::basic_fields<alloc_t>>> string_serializer_;

	// The file-based response message.
	boost::optional<http::response<http::file_body, http::basic_fields<alloc_t>>> file_response_;

	// The file-based response serializer.
	boost::optional<
			http::response_serializer<http::file_body,
					http::basic_fields<alloc_t>>> file_serializer_;

	void accept() {
		// Clean up any previous connection.
		boost::beast::error_code ec;
		socket_.close(ec);
		buffer_.consume(buffer_.size());

		acceptor_.async_accept(socket_, [this](boost::beast::error_code ec)
		{
			if (ec)
			{
				accept();
			}
			else
			{
				// Request must be fully processed within 60 seconds.
				request_deadline_.expires_after(
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

		http::async_read(socket_, buffer_, *parser_,
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

		string_serializer_.emplace(*string_response_);

		http::async_write(socket_, *string_serializer_,
				[this](boost::beast::error_code ec, std::size_t)
				{
					socket_.shutdown(tcp::socket::shutdown_send, ec);
					string_serializer_.reset();
					string_response_.reset();
					accept();
				});
	}

	void send_file(boost::beast::string_view target) {
		// Request path must be absolute and not contain "..".
		if (target.empty() || target[0] != '/'
				|| target.find("..") != std::string::npos) {
			send_bad_response(http::status::not_found, "File not found\r\n");
			return;
		}

		std::string full_path = doc_root_;
		full_path.append(target.data(), target.size());

		http::file_body::value_type file;
		boost::beast::error_code ec;
		file.open(full_path.c_str(), boost::beast::file_mode::read, ec);
		if (ec) {
			send_bad_response(http::status::not_found, "File not found\r\n");
			return;
		}

		file_response_.emplace(std::piecewise_construct, std::make_tuple(),
				std::make_tuple(alloc_));

		file_response_->result(http::status::ok);
		file_response_->keep_alive(false);
		file_response_->set(http::field::server, "Beast");
		file_response_->set(http::field::content_type,
				mime_type(target.to_string()));
		file_response_->body() = std::move(file);
		file_response_->prepare_payload();

		file_serializer_.emplace(*file_response_);

		http::async_write(socket_, *file_serializer_,
				[this](boost::beast::error_code ec, std::size_t)
				{
					socket_.shutdown(tcp::socket::shutdown_send, ec);
					file_serializer_.reset();
					file_response_.reset();
					accept();
				});
	}

	void check_deadline() {
		// The deadline may have moved, so check it has really passed.
		if (request_deadline_.expiry() <= std::chrono::steady_clock::now()) {
			// Close socket to cancel any outstanding operation.
			boost::beast::error_code ec;
			socket_.close();

			// Sleep indefinitely until we're given a new deadline.
			request_deadline_.expires_at(
					std::chrono::steady_clock::time_point::max());
		}

		request_deadline_.async_wait([this](boost::beast::error_code)
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
		boost::string_view configurationFilePathView = "../../connect.json";
		boost::optional<boost::string_view> hostAddress;
		boost::optional<int> hostPort;
		boost::optional<boost::string_view> hostDocumentRoot;
		boost::optional<int> hostWorkerNum;
		boost::optional<bool> hostPoll;
		if (argc > 1) {
			configurationFilePathView = argv[1];
		}
		boost::filesystem::path configurationFilePath {
				configurationFilePathView.data() };
		boost::filesystem::ifstream configurationFileStream {
				configurationFilePath };
		if (configurationFileStream.is_open()) {
			std::cout << "Opened the configuration file\n" << "\""
					<< configurationFilePathView.data() << "\"\n";
		} else {
			throw myException("Could not open the configuration file");
		}
		rapidjson::IStreamWrapper isw(configurationFileStream);
		rapidjson::EncodedInputStream<rapidjson::UTF8<>,
				rapidjson::IStreamWrapper> eis(isw);
		rapidjson::Document jsonDocument; // Document is GenericDocument<UTF8<> >
		jsonDocument.ParseStream<
				rapidjson::kParseNanAndInfFlag
						+ rapidjson::kParseTrailingCommasFlag
						+ rapidjson::kParseCommentsFlag, rapidjson::UTF8<> >(
				eis); // Parses UTF-8 file into UTF-8 in memory
		if (jsonDocument.HasParseError()) {
			std::cout << "Failed to parse JSON!";
			if (jsonDocument.GetParseError() == 3) {
				std::cout << " Possible reason: file encoding incorrect\n";
			} else {
				std::cout << " Error code: " << jsonDocument.GetParseError()
						<< " at position " << jsonDocument.GetErrorOffset()
						<< "\n";
			}
		} else if (!jsonDocument.IsObject()) {
			std::cout << "JSON is not an object\n";
		} else {
			if (jsonDocument.HasMember("address")
					&& jsonDocument["address"].IsString()) {
				hostAddress = boost::string_view(
						jsonDocument["address"].GetString());
			}
			if (jsonDocument.HasMember("port")
					&& jsonDocument["port"].IsInt()) {
				hostPort = jsonDocument["port"].GetInt();
			}
			if (jsonDocument.HasMember("documentRoot")
					&& jsonDocument["documentRoot"].IsString()) {
				hostDocumentRoot = boost::string_view(
						jsonDocument["documentRoot"].GetString());
			}
			if (jsonDocument.HasMember("workers")
					&& jsonDocument["workers"].IsInt()) {
				hostWorkerNum = jsonDocument["workers"].GetInt();
			}
			if (jsonDocument.HasMember("poll")
					&& jsonDocument["poll"].IsBool()) {
				hostPoll = jsonDocument["poll"].GetBool();
			}
		}
		if (hostAddress && hostPort && hostDocumentRoot && hostWorkerNum
				&& hostPoll) {
			std::cout
					<< "Found the required information from the configuration file.\n";
		} else {
			throw myException(
					"Could not get the required information from the configuration file");
		}

		auto const address = boost::asio::ip::make_address(
				(*hostAddress).data());
		unsigned short port = static_cast<unsigned short>(*hostPort);

		boost::asio::io_context ioc { 1 };
		tcp::acceptor acceptor { ioc, { address, port } };

		std::list<http_worker> workers;
		for (int i = 0; i < *hostWorkerNum; ++i) {
			workers.emplace_back(acceptor, (*hostDocumentRoot).data());
			workers.back().start();
		}

		if (*hostPoll)
			for (;;)
				ioc.poll();
		else
			ioc.run();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << "Enter any key to exit...\n";
		std::getline(std::cin, temp);
		return EXIT_FAILURE;
	}
	std::cout << "Enter any key to exit...\n";
	std::getline(std::cin, temp);
	return EXIT_SUCCESS;
}
