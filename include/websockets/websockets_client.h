#pragma once

#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/websockets_endpoint.h"

class WebSocketsClient : private WebSocketsEndpoint {
public:
	WebSocketsClient(TcpClient* client);

	bool connect(String host, int port);
	bool poll(String& data);
	bool available();
	void send(String data);

	~WebSocketsClient();

private:
	TcpClient* _client;
	WebsocketsFrame recvFrame();
};