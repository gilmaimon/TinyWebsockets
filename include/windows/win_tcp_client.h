#pragma once

#include "common.h"
#include "tcp_client.h"

#include <windows.h>

class WinTcpSocket : public TcpSocket {
public:
	bool connect(String host, int port);
	bool poll() override;
	bool available() override;
	void send(String data) override;
	void send(uint8_t* data, uint32_t len) override;
	String readLine() override;
	void read(uint8_t* buffer, uint32_t len) override;
	virtual ~WinTcpSocket();

private:
	SOCKET socket;
};

class WinTcpClient : public TcpClient {
public:
	bool connect(String host, int port) {
		return socket.connect(host, port);
	}

	bool poll() {
		return socket.poll();
	}

	bool available() override {
		return socket.available();
	}

	void send(String data) override {
		socket.send(data);
	}

	void send(uint8_t* data, uint32_t len) override {
		socket.send(data, len);
	}
	
	String readLine() override {
		return socket.readLine();
	}

	void read(uint8_t* buffer, uint32_t len) override {
		socket.read(buffer, len);
	}

	virtual ~WinTcpClient() {}
private:
	WinTcpSocket socket;
};
