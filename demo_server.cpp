#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <iostream>

using namespace websockets;

int main() {
	WebsocketsServer server;
	server.listen(8081);
	while(server.available()) {
		WebsocketsClient client = server.accept();
		auto message = client.readBlocking();
		client.send("Echo: " + message.data());
		client.close();
	}
}