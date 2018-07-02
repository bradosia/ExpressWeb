#include <iostream>
#include <ExpressWeb.h>

int main() {
	ExpressWeb::Server server;
	server.connection(3000);
	server.start();
	return 0;
}
