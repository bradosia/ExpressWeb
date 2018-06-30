#include <iostream>
#include <ExpressWeb.h>

int main() {
	ExpressWeb::Server server;
	server.connection(3000);
	server.start([](ExpressWeb::Error_code ec)-> {
		if (ec) {
			throw ec;
		}
		std::cout << "Server running at: " << server.info.uri << "\n";
	});
	return 0;
}
