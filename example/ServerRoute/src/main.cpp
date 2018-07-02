#include <iostream>
#include <ExpressWeb.h>

int main() {
	ExpressWeb::Server server(3000, "localhost");

	server.route("GET", "/",
			[](ExpressWeb::Request& request, ExpressWeb::Response& response) {
				response.send("Hello, world!");
				return true;
			});

	server.start();
	return 0;
}
