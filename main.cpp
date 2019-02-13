#include "websockets/websockets_client.h"
#include "network/windows/win_tcp_client.h"
#include <iostream>

using namespace websockets;

int main() {
	WebsocketsClient client(new network::WinTcpClient);
	client.connect("localhost", "/", 8080);

	client.onMessage([&](WebsocketsMessage message){
		std::cout << "Got Data: " << message.data() << std::endl;
	});

	String data;
	while(client.available()) {
		std::cout << "Enter input: ";
		std::getline(std::cin, data);
		
		if(data.size() > 0) {
			if(data == "exit") client.close();
			else client.send(data);
		}

		client.poll();
	}
	std::cout << "Exited Gracefully" << std::endl;	
}