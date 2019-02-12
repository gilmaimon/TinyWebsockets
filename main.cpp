#include "websockets/websockets_client.h"
#include "windows/win_tcp_client.h"

#include <thread>
int main() {
	WebSocketsClient client(new WinTcpClient);
	client.connect("localhost", 8080);

	client.onMessage([&](WebsocketsMessage message){
		std::cout << "Got Data: " << message.data() << std::endl;
	});

	String data;
	while(true) {
		std::cin >> data;
		if(data != "skip") 
			client.send(data);
		
		client.poll();
	}
	system("pause");
}