#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
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
using headerMap_t = std::unordered_map<std::string, boost::string_view>;
// from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;
// from <boost/beast/websocket.hpp>

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
int main(int argc, char** argv) {
	std::string temp;
	try {
		boost::string_view configurationFilePathView = "../../connect.json";
		boost::optional<boost::string_view> clientHost;
		boost::optional<boost::string_view> clientPort;
		boost::optional<boost::string_view> clientScheme;
		boost::optional<boost::string_view> clientTarget;
		boost::optional<std::string> clientRequest;
		boost::optional<headerMap_t> clientHeaders;
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
					rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
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

		// These objects perform our I/O
		tcp::resolver resolver { ioc };
		websocket::stream<tcp::socket> websocketStream { ioc };

		// Look up the domain name
		auto const results = resolver.resolve((*clientHost).data(),
				(*clientPort).data());

		// Make the connection on the IP address we get from a lookup
		boost::asio::connect(websocketStream.next_layer(), results.begin(),
				results.end());

		// Perform the websocket handshake
		websocket::response_type HTTP_response;
		websocketStream.handshake_ex(HTTP_response, *clientHost, *clientTarget,
				[&](websocket::request_type& HTTP_request)
				{
					std::string hostAuthority = std::string((*clientHost).data()).append(":").append(
							(*clientPort).data());
					// websocket-sharp authentication fails unless the port is present for a non standard host port
					HTTP_request.set("Host", (*clientPort == "80" && *clientScheme == "ws") || (*clientPort == "443" && *clientScheme == "wss")
							? *clientHost
							: hostAuthority);
					// An optional header map defined in the configuration file
					if(clientHeaders) {
						for(auto i : *clientHeaders)
						{
							HTTP_request.insert(i.first, i.second);
						}
					}
					// Let's see what our HTTP request is
					std::cout << "HTTP Request Headers:\n";
					std::cout << HTTP_request << "\n";
				});
		// Let's see what our HTTP response is
		std::cout << "HTTP Response Headers:\n";
		std::cout << HTTP_response << "\n";

		// Send the message
		websocketStream.write(
				boost::asio::buffer(std::string((*clientRequest).data())));

		// This buffer will hold the incoming message
		boost::beast::multi_buffer buffer;

		// Read a message into our buffer
		websocketStream.read(buffer);

		// Close the WebSocket connection
		websocketStream.close(websocket::close_code::normal);

		// If we get here then the connection is closed gracefully

		// The buffers() function helps print a ConstBufferSequence
		std::cout << boost::beast::buffers(buffer.data()) << std::endl;
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
