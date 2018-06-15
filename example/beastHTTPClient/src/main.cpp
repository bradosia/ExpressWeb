#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>		// linux-x86_64-gcc needs this for boost::filesystem::fstream
#include <cstdlib>
#include <iostream>
#include <string>

#ifndef ONE_HEADER
#include <rapidjson.h>
#endif

using tcp = boost::asio::ip::tcp;
// from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;
// from <boost/beast/http.hpp>

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

// Performs an HTTP GET and prints the response
int main(int argc, char** argv) {
	std::string temp;
	try {
		boost::string_view configurationFilePathView = "../../connect.json";
		boost::optional<boost::string_view> clientHost;
		boost::optional<boost::string_view> clientPort;
		boost::optional<boost::string_view> clientTarget;
		boost::optional<int> clientVersion;
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
			if (jsonDocument.HasMember("host")
					&& jsonDocument["host"].IsString()) {
				clientHost = boost::string_view(
						jsonDocument["host"].GetString());
			}
			if (jsonDocument.HasMember("port")
					&& jsonDocument["port"].IsString()) {
				clientPort = boost::string_view(
						jsonDocument["port"].GetString());
			}
			if (jsonDocument.HasMember("target")
					&& jsonDocument["target"].IsString()) {
				clientTarget = boost::string_view(
						jsonDocument["target"].GetString());
			}
			if (jsonDocument.HasMember("version")
					&& jsonDocument["version"].IsInt()) {
				clientVersion = jsonDocument["version"].GetInt();
			}
		}
		if (clientHost && clientPort && clientTarget && clientVersion) {
			std::cout
					<< "Found the required information from the configuration file.\n";
		} else {
			throw myException(
					"Could not get the required information from the configuration file");
		}

		// The io_context is required for all I/O
		boost::asio::io_context ioc;

		// These objects perform our I/O
		tcp::resolver resolver { ioc };
		tcp::socket socket { ioc };

		// Look up the domain name
		auto const results = resolver.resolve((*clientHost).data(),
				(*clientPort).data());

		// Make the connection on the IP address we get from a lookup
		boost::asio::connect(socket, results.begin(), results.end());

		// Set up an HTTP GET request message
		http::request<http::string_body> req { http::verb::get, *clientTarget,
				*clientVersion };
		req.set(http::field::host, *clientHost);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		// Send the HTTP request to the remote host
		http::write(socket, req);

		// This buffer is used for reading and must be persisted
		boost::beast::flat_buffer buffer;

		// Declare a container to hold the response
		http::response<http::dynamic_body> res;

		// Receive the HTTP response
		http::read(socket, buffer, res);

		// Write the message to standard out
		std::cout << res << std::endl;

		// Gracefully close the socket
		boost::system::error_code ec;
		socket.shutdown(tcp::socket::shutdown_both, ec);

		// not_connected happens sometimes
		// so don't bother reporting it.
		//
		if (ec && ec != boost::system::errc::not_connected)
			throw boost::system::system_error { ec };

		// If we get here then the connection is closed gracefully
	} catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cout << "Enter any key to exit...\n";
		std::getline(std::cin, temp);
		return EXIT_FAILURE;
	}
	std::cout << "Enter any key to exit...\n";
	std::getline(std::cin, temp);
	return EXIT_SUCCESS;
}
