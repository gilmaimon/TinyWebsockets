#pragma once

#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"

class WebSocketsClient {
public:
	WebSocketsClient(TcpClient* client);

	bool connect(String host, int port);
	void poll();
	bool available();

	~WebSocketsClient();

private:
	TcpClient* _client;
	WebsocketsFrame recvFrame();
};