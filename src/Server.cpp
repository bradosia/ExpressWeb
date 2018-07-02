#include "Server.h"

namespace ExpressWeb {

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

void Server::setStaticServe(bool staticServe) {
	staticServe_ = staticServe;
}

void Server::connection(unsigned short port) {
	setPort(port);
}

void Server::onClientError(callBackClientError_t cb) {
	callBackClientError = cb;
}
void Server::onClose(callBackClose_t cb) {
	callBackClose = cb;
}
void Server::onConnection(callBackConnection_t cb) {
	callBackConnection = cb;
}
void Server::onRequest(callBackRequest_t cb) {
	callBackRequest = cb;
}

void Server::start() {
	Server::start(callBackClientError_t { });
}

void Server::start(callBackClientError_t cb) {
	onClientError(cb);
	// set defaults
	if (!port_) {
		setPort(80);
	}
	if (!address_) {
		setAddress("localhost");
	}
	if (!poll_) {
		setPoll(false);
	}
	// set default worker number
	if (!workerNum_) {
		setWorkerNum(4);
	}
	if (!documentRoot_) {
		setDocumentRoot("/");
	}

	boost::asio::io_context ioc { 1 };
	boost::asio::ip::tcp::acceptor acceptor { ioc,
			{ address_.get(), port_.get() } };

	unsigned int i, n;
	n = workerNum_.get();
	std::list<ExpressWeb::Http::Worker> workers;
	for (i = 0; i < n; ++i) {
		workers.emplace_back(acceptor);
		if (callBackClientError) {
			workers.back().onClientError(callBackClientError.get());
		}
		if (callBackClose) {
			workers.back().onClose(callBackClose.get());
		}
		if (callBackConnection) {
			workers.back().onConnection(callBackConnection.get());
		}
		if (callBackRequest) {
			workers.back().onRequest(callBackRequest.get());
		}
		if (staticServe_ && documentRoot_) {
			workers.back().setStaticServe(true);
			workers.back().setDocumentRoot(documentRoot_.get());
		}
		workers.back().start();
	}

	if (poll_.get())
		for (;;)
			ioc.poll();
	else
		ioc.run();
}

} // namespace ExpressWeb
