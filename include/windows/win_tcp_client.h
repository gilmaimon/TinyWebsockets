#pragma once

#include "common.h"
#include "tcp_client.h"

#include <windows.h>

class WinTcpClient : public TcpClient {
public:
	bool connect(String host, int port) override;
	bool available() override;
	void send(String data) override;
	void send(uint8_t* data, uint32_t len) override;
	String readLine() override;
	void read(uint8_t* buffer, uint32_t len) override;
	virtual ~WinTcpClient();

private:
	SOCKET socket;
};
