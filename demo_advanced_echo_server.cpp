/*
	Single threaded Echo Server that can handle multiple clients.
	
	After running this demo, there will be a websockets server running
	on port SERVER_PORT (default 8080), that server will accept connections
	and will respond with "echo" messages for every received message.

	The code:
		1. Sets up a server
		2. While possible, checks if there is a client wanting to connect
			2-1. If there is, accept the connection
			2-2. Set up callbacks (for incoming messages)
			2-3. Store the client in a collection for future use
		3. For every client in the collection, poll for incoming events

		When a message is received: respond to that message with `"Echo: " + message.data()`
*/

#define SERVER_PORT 8080

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace websockets;

// a collection of all connected clients
std::vector<WebsocketsClient> allClients;

// this method goes thrugh every client and polls for new messages and events
void pollAllClients() {
	for(auto& client : allClients) {
		client.poll();
	}
}

// this callback is common for all clients, the client that sent that
// message is the one that gets the echo response
void onMessage(WebsocketsClient& client, WebsocketsMessage message) {
	std::cout << "Got Message: `" << message.data() << "`, Sending Echo." << std::endl;
	client.send("Echo: " + message.data());
}


int main() {
	WebsocketsServer server;
	server.listen(SERVER_PORT);

	// while the server is alive
	while(server.available()) {

		// if there is a client that wants to connect
		if(server.poll()) {
			//accept the connection and register callback
			std::cout << "Accepting a new client!" << std::endl;
			WebsocketsClient client = server.accept();
			client.onMessage(onMessage);

			// store it for later use
			allClients.push_back(client);
		}

		// check for updates in all clients
		pollAllClients();
	}
}