#include "websockets/websockets_client.h"
#include "windows/win_tcp_client.h"

#include <thread>
int main() {
	WebSocketsClient client(new WinTcpClient);
	client.connect("localhost", 8080);

	client.setMessageHandler([&](WebsocketsMessage message){
		if(message.isText()) {
			std::cout << "Got Text: " << message.data() << std::endl;
		} else {
			std::cout << "Else. " << std::endl;
		}
	});

	while(true) {
		String data;
		std::cin >> data;
		if(data != "skip") client.send(data);
		client.poll();
	}
	system("pause");
}