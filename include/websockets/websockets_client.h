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
	void onMessage(MessageCallback callback);
	void poll();
	bool available(bool activeTest = false);

	void send(String data);
	void sendBinary(String data);

	void close();

	~WebSocketsClient();

private:
	TcpClient* _client;
	MessageCallback _callback;
	bool _connectionOpen;

	void _handlePing(WebsocketsMessage);
	void _handlePong(WebsocketsMessage);
	void _handleClose(WebsocketsMessage);
};