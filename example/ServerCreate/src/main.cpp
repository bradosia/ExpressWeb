#include <iostream>
#include <ExpressWeb.h>

int main() {
	ExpressWeb::Server server();
	server.connection( { "port", 3000 });
	server.start([](err) {
		if (err) {
			throw err;
		}
		std::cout << "Server running at: " << server.info.uri << "\n";
	});
	return 0;
}
