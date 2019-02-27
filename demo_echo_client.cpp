/*
	Interactive and basic Websockets Client that connect to a public echo server
	
	After running this demo, there will be a websockets client connected
	to `echo.websocket.org` and every message the user will enter will be 
	sent to the server. Incoming messages will be printed once the user 
	enters an input (or an empty line, just an Enter)
	Enter "exit" to close the connection and end the program.

	The code:
		1. Sets up a client connection
		2. Reads an input from the user
			2-1. If the user didnt enter an empty line, the client sends the message to the server
			2-2. If the user enters "exit", the program closes the connection
		3. Polls for incoming messages and events.
*/

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <iostream>

using namespace websockets;

 int main() {
 	WebsocketsClient client;
	client.connect("ws://echo.websocket.org/");

	client.onMessage([&](WebsocketsClient&, WebsocketsMessage message){
		std::cout << "Got Data: " << message.data() << std::endl;
	});

	WSString line;
	while(client.available()) {
		std::cout << "Enter input: ";
		std::getline(std::cin, line);

		if(line != "") {
			if(line == "exit") client.close();
			else {
				client.poll();
				client.send(line);
			}
		}
		client.poll();
	}
	std::cout << "Exited Gracefully" << std::endl;
}