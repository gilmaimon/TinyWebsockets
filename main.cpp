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

// void _do() {
// 	network::WinTcpServer server;
// 	server.listen("localhost", 8081);
// 	if(!server.available()) {
// 		return;
// 	}

// 	auto client = server.accept();
// 	system("sleep 1");
// 	while(client->poll()) std::cout << client->readLine();
// 	delete client;
// }
// int main() {
// 	_do();
// 	return 1;
// 	WebsocketsClient client;

// 	//WebsocketsServer server;
// 	//server.listen("localhost", 8081);

// 	client.onMessage([&](WebsocketsMessage message){
// 		std::cout << "Got Data: " << message.data() << std::endl;
// 	});

// 	client.onEvent([&](WebsocketsEvent event, auto data){
// 		switch(event) {
// 			case WebsocketsEvent::ConnectionOpened: {
// 				std::cout << "Connection Opened!";
// 			}
// 			break;
// 			case WebsocketsEvent::GotPing: {
// 				std::cout << "Got Ping";
// 			}
// 			break;
// 			case WebsocketsEvent::GotPong: {
// 				std::cout << "Got Pong";
// 			}
// 			break;
// 			case WebsocketsEvent::ConnectionClosed: {
// 				std::cout << "Connection Closed!";
// 			}
// 			break;
// 		}

// 		std::cout << ", With Data: " << data << std::endl; 
// 	});

// 	client.connect("http://echo.websocket.org:80/");
// 	client.ping("Ping Message");

// 	WSString data;
// 	while(client.available()) {
// 		std::cout << "Enter input: ";
// 		std::getline(std::cin, data);
		
// 		if(data.size() > 0) {
// 			if(data == "exit") client.close();
// 			else {
// 				client.poll();
// 				client.send(data);
// 			}
// 		}

// 		client.poll();
// 	}
// 	std::cout << "Exited Gracefully" << std::endl;	
// }