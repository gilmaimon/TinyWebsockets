#include <tiny_websockets/client.hpp>
#include <iostream>

using namespace websockets;

int main() {
	WebsocketsClient client;

	client.onMessage([&](WebsocketsMessage message){
		std::cout << "Got Data: " << message.data() << std::endl;
	});

	client.onEvent([&](WebsocketsEvent event, WSString data){
		switch(event) {
			case WebsocketsEvent::ConnectionOpened: {
				std::cout << "Connection Opened!";
			}
			break;
			case WebsocketsEvent::GotPing: {
				std::cout << "Got Ping";
			}
			break;
			case WebsocketsEvent::GotPong: {
				std::cout << "Got Pong";
			}
			break;
			case WebsocketsEvent::ConnectionClosed: {
				std::cout << "Connection Closed!";
			}
			break;
		}

		std::cout << ", With Data: " << data << std::endl; 
	});

	client.connect("http://localhost:8080/");
	client.ping("Ping Message");

	WSString data;
	while(client.available()) {
		std::cout << "Enter input: ";
		std::getline(std::cin, data);
		
		if(data.size() > 0) {
			if(data == "exit") client.close();
			else {
				client.poll();
				client.send(data);
			}
		}

		client.poll();
	}
	std::cout << "Exited Gracefully" << std::endl;	
}