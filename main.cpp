#include "websockets/websockets_client.h"
#include "windows/win_tcp_client.h"

int main() {
	WebSocketsClient client(new WinTcpClient);
	client.connect("localhost", 8080);
	client.poll();
	client.poll();

	system("pause");
}