//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket client, asynchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#ifndef ONE_HEADER
#include <rapidjson.h>
#endif

using tcp = boost::asio::ip::tcp;
using headerMap_t = std::unordered_map<std::string, boost::string_view>;
// from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;
// from <boost/beast/websocket.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(boost::system::error_code ec, char const* what) {
	std::cerr << what << ": " << ec.message() << "\n";
}

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

// Sends a WebSocket message and prints the response
class session: public std::enable_shared_from_this<session> {
private:
	tcp::resolver TCP_resolver_;
	websocket::stream<tcp::socket> websocketStream;
	boost::beast::multi_buffer buffer_;
	boost::string_view host_;
	boost::string_view port_;
	boost::string_view scheme_;
	boost::string_view target_;
	std::string requestMessage_;
	std::string hostAuthority;
	boost::optional<headerMap_t> headerMap_;
	websocket::response_type HTTP_response;

public:
	// Resolver and socket require an io_context
	explicit session(boost::asio::io_context& ioc) :
			TCP_resolver_(ioc), websocketStream(ioc) {
	}

	// Start the asynchronous operation
	void run(boost::string_view& host, boost::string_view& port,
			boost::string_view& scheme, boost::string_view& target,
			std::string& requestMessage,
			boost::optional<headerMap_t>& headerMap) {
		// Save these for later
		host_ = host;
		port_ = port;
		scheme_ = scheme;
		hostAuthority = std::string(host.data()).append(":").append(
				port.data());
		target_ = target;
		requestMessage_ = requestMessage;
		headerMap_ = headerMap;

		// Look up the domain name
		TCP_resolver_.async_resolve(host.data(), port.data(),
				std::bind(&session::on_resolve, shared_from_this(),
						std::placeholders::_1, std::placeholders::_2));
	}

	void on_resolve(boost::system::error_code ec,
			tcp::resolver::results_type results) {
		if (ec)
			return fail(ec, "resolve");

		// Make the connection on the IP address we get from a lookup
		boost::asio::async_connect(websocketStream.next_layer(),
				results.begin(), results.end(),
				std::bind(&session::on_connect, shared_from_this(),
						std::placeholders::_1));
	}

	void on_connect(boost::system::error_code ec) {
		if (ec)
			return fail(ec, "connect");

		// Perform the websocket handshake
		websocketStream.async_handshake_ex(HTTP_response, host_, target_,
				[this](websocket::request_type& HTTP_request)
				{
					// websocket-sharp authentication fails unless the port is present for a non standard host port
					HTTP_request.set("Host", (port_ == "80" && scheme_ == "ws") || (port_ == "443" && scheme_ == "wss")
							? host_
							: hostAuthority);
					// An optional header map defined in the configuration file
					if(headerMap_) {
						for(auto i : *headerMap_)
						{
							HTTP_request.insert(i.first, i.second);
						}
					}
					// Let's see what our HTTP request is
					std::cout << "HTTP Request Headers:\n";
					std::cout << HTTP_request << "\n";
				},
				std::bind(&session::on_handshake, shared_from_this(),
						std::placeholders::_1));

	}

	void on_handshake(boost::system::error_code ec) {
		// Let's see what our HTTP response is
		std::cout << "HTTP Response Headers:\n";
		std::cout << HTTP_response << "\n";
		if (ec)
			return fail(ec, "handshake");

		// Send the message
		websocketStream.async_write(boost::asio::buffer(requestMessage_),
				std::bind(&session::on_write, shared_from_this(),
						std::placeholders::_1, std::placeholders::_2));
	}

	void on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "write");

		// Read a message into our buffer
		websocketStream.async_read(buffer_,
				std::bind(&session::on_read, shared_from_this(),
						std::placeholders::_1, std::placeholders::_2));
	}

	void on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return fail(ec, "read");

		// Close the WebSocket connection
		websocketStream.async_close(websocket::close_code::normal,
				std::bind(&session::on_close, shared_from_this(),
						std::placeholders::_1));
	}

	void on_close(boost::system::error_code ec) {
		if (ec)
			return fail(ec, "close");

		// If we get here then the connection is closed gracefully

		// The buffers() function helps print a ConstBufferSequence
		std::cout << boost::beast::buffers(buffer_.data()) << std::endl;
	}
};

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
	std::string temp;
	boost::string_view configurationFilePathView = "../../connect.json";
	boost::optional < boost::string_view > clientHost;
	boost::optional < boost::string_view > clientPort;
	boost::optional < boost::string_view > clientScheme;
	boost::optional < boost::string_view > clientTarget;
	boost::optional<std::string> clientRequest;
	boost::optional<headerMap_t> clientHeaders;
	if (argc > 1) {
		configurationFilePathView = argv[1];
	}
	boost::filesystem::path configurationFilePath {
			configurationFilePathView.data() };
	boost::filesystem::ifstream configurationFileStream { configurationFilePath };
	if (configurationFileStream.is_open()) {
		std::cout << "Opened the configuration file\n" << "\""
				<< configurationFilePathView.data() << "\"\n";
	} else {
		throw myException("Could not open the configuration file");
	}
	rapidjson::IStreamWrapper isw(configurationFileStream);
	rapidjson::EncodedInputStream<rapidjson::UTF8<>, rapidjson::IStreamWrapper> eis(
			isw);
	rapidjson::Document jsonDocument; // Document is GenericDocument<UTF8<> >
	jsonDocument.ParseStream<
			rapidjson::kParseNanAndInfFlag + rapidjson::kParseTrailingCommasFlag
					+ rapidjson::kParseCommentsFlag, rapidjson::UTF8<> >(eis); // Parses UTF-8 file into UTF-8 in memory
	if (jsonDocument.HasParseError()) {
		std::cout << "Failed to parse JSON!";
		if (jsonDocument.GetParseError() == 3) {
			std::cout << " Possible reason: file encoding incorrect\n";
		} else {
			std::cout << " Error code: " << jsonDocument.GetParseError()
					<< " at position " << jsonDocument.GetErrorOffset() << "\n";
		}
	} else if (!jsonDocument.IsObject()) {
		std::cout << "JSON is not an object\n";
	} else {
		if (jsonDocument.HasMember("host") && jsonDocument["host"].IsString()) {
			clientHost = boost::string_view(jsonDocument["host"].GetString());
		}
		if (jsonDocument.HasMember("port") && jsonDocument["port"].IsString()) {
			clientPort = boost::string_view(jsonDocument["port"].GetString());
		}
		if (jsonDocument.HasMember("scheme")
				&& jsonDocument["scheme"].IsString()) {
			clientScheme = boost::string_view(
					jsonDocument["scheme"].GetString());
		}
		if (jsonDocument.HasMember("target")
				&& jsonDocument["target"].IsString()) {
			clientTarget = boost::string_view(
					jsonDocument["target"].GetString());
		}
		if (jsonDocument.HasMember("request")) {
			if (jsonDocument["request"].IsObject()) {
				// If request is an object, then serialize as a string
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer < rapidjson::StringBuffer > writer(buffer);
				jsonDocument["request"].Accept(writer);
				clientRequest = buffer.GetString();
			} else if (jsonDocument["request"].IsString()) {
				clientRequest = jsonDocument["request"].GetString();
			}
		}
		if (jsonDocument.HasMember("headers")
				&& jsonDocument["headers"].IsObject()) {
			// construct the optional header map
			headerMap_t headerMap;
			for (auto& m : jsonDocument["headers"].GetObject()) {
				if (m.name.IsString() && m.value.IsString()) {
					headerMap.emplace(m.name.GetString(),
							boost::string_view(m.value.GetString()));
				}
			}
			clientHeaders = headerMap;
		}
	}
	if (clientHost && clientPort && clientTarget && clientRequest) {
		std::cout
				<< "Found the required information from the configuration file.\n";
	} else {
		throw myException(
				"Could not get the required information from the configuration file");
	}

	std::cout << *clientRequest << std::endl;

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	// Launch the asynchronous operation
	std::make_shared<session>(ioc)->run(*clientHost, *clientPort, *clientScheme,
			*clientTarget, *clientRequest, clientHeaders);

	// Run the I/O service. The call will return when
	// the socket is closed.
	ioc.run();

	std::cout << "Enter any key to exit...\n";
	std::getline(std::cin, temp);
	return EXIT_SUCCESS;
}
