#pragma once
#include "common.h"

struct TcpClient {
	virtual bool connect(String host, int port) = 0;
	virtual bool available() = 0;
	virtual void send(String data) = 0;
	virtual void send(uint8_t* data, uint32_t len) = 0;
	virtual String readLine() = 0;
	virtual void read(uint8_t* buffer, uint32_t len) = 0;
};