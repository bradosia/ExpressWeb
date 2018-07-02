#include "Http.h"

namespace ExpressWeb {

namespace Http {

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

void Worker::setStaticServe(bool staticServe) {
	staticServe_ = staticServe;
}

void Worker::onClientError(callBackClientError_t cb) {
	callBackClientError = cb;
}
void Worker::onClose(callBackClose_t cb) {
	callBackClose = cb;
}
void Worker::onConnection(callBackConnection_t cb) {
	callBackConnection = cb;
}
void Worker::onRequest(callBackRequest_t cb) {
	callBackRequest = cb;
}


} // namespace http

} // namespace ExpressWeb
