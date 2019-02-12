#pragma once

#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/websockets_endpoint.h"
#include "websockets/message.h"

class WebSocketsClient : private WebSocketsEndpoint {
public:
	WebSocketsClient(TcpClient* client);

	bool connect(String host, int port);
	void setMessageHandler(MessageCallback callback);
	void poll();
	bool available();
	void send(String data);

	~WebSocketsClient();

private:
	TcpClient* _client;
	MessageCallback _callback;
};