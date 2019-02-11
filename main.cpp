#include "websockets/websockets_client.h"
#include "windows/win_tcp_client.h"

#include <thread>
int main() {
	WebSocketsClient client(new WinTcpClient);
	client.connect("localhost", 8080);

	while(true) {
		String data;
		std::cin >> data;
		
		if(data != "skip") client.send(data);
		if(client.poll(data)) {
			std::cout << "Got: " << data << std::endl;
		}
	}
	system("pause");
}