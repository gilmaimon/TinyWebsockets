#include <string>
#include <iostream>
#include <bitset> // for logging
#include <memory.h>

typedef std::string String;

struct TcpClient {
	virtual bool connect(String host, int port) = 0;
	virtual bool available() = 0;
	virtual void send(String data) = 0;
	virtual void send(uint8_t* data, uint32_t len) = 0;
	virtual String readLine() = 0;
	virtual void read(uint8_t* buffer, uint32_t len) = 0;
};

struct WebsocketsFrame {
	uint8_t fin : 1;
	uint8_t opcode : 4;
	uint8_t mask : 1;
	uint8_t mask_buf[4];
	uint64_t payload_length;
	String payload;

	void log() {
		std::cout << "Frame:" << std::endl;
		std::cout << "-> fin: " << (int)fin << std::endl;
		std::cout << "-> opcode: " << std::hex << (int)opcode << std::dec << " (" << std::bitset<4>(opcode) << ")" << std::endl;
		std::cout << "-> len: " << payload_length << " (" << std::bitset<64>(payload_length) << ")" << std::endl;
		std::cout << "-> mask: " << (int)mask << std::endl;
		std::cout << "-> payload: " << payload << std::endl;
	}
};

class WebSocketsClient {
public:
	WebSocketsClient(TcpClient* client) : _client(client) {
		// Empty
	}

	WebsocketsFrame recvFrame() {
		struct Header {
			uint8_t opcode : 4;
			uint8_t flags : 3;
			uint8_t fin : 1;
			uint8_t payload : 7;
			uint8_t mask : 1;
		
			void log() {
				std::cout << "Header:" << std::endl;
				std::cout << "-> fin: " <<  (int)fin << std::endl;
				std::cout << "-> flags: " << std::bitset<3>(flags) << std::endl;
				std::cout << "-> opcode: " << std::hex << (int) opcode << std::dec << " (" << std::bitset<4>(opcode) << ")" << std::endl;
				std::cout << "-> payload: " << (int)payload << " (" << std::bitset<7>(payload) << ")" << std::endl;
				std::cout << "-> mask: " << (int)mask << std::endl;
			}
		};

		Header header;
		header.payload = 0;

		//std::cout << "Reading header" << std::endl;
		
		// read common header
		this->_client->read((uint8_t*)&header, 2);
		//header.log();

		uint64_t extendedPayload = header.payload;
		// in case of extended payload length
		if (header.payload == 126) {
			// read next 16 bits as payload length
			uint16_t tmp = 0;
			this->_client->read((uint8_t*)&tmp, 2);
			tmp = (tmp << 8) | (tmp >> 8);
			extendedPayload = tmp;
		}
		else if (header.payload == 127) {
			// TODO: read next 64 bits as payload length and handle such very long messages
		}
		//std::cout << "extendedPayload: " << extendedPayload << std::endl;

		uint8_t maskingKey[4];

		// if masking is set
		if (header.mask) {
			// read the masking key
			this->_client->read((uint8_t*)maskingKey, 1);
		}

		// read the message's payload (data) according to the read length
		const uint64_t BUFFER_SIZE = 64;
		String data = "";
		uint8_t buffer[BUFFER_SIZE];
		uint64_t done_reading = 0;
		while (done_reading < extendedPayload) {
			uint64_t to_read = extendedPayload - done_reading >= BUFFER_SIZE ? BUFFER_SIZE : extendedPayload - done_reading;
			this->_client->read(buffer, to_read);
			done_reading += to_read;

			for (int i = 0; i < to_read; i++) {
				data += (char)buffer[i];
			}
			//std::cout << "Building data: " << data << std::endl;
		}

		// if masking is set
		if (header.mask) {
			// un-mask the message
			for (int i = 0; i < extendedPayload; i++) {
				data[i] = data[i] & maskingKey[i % 4];
			}
		}

		WebsocketsFrame frame;
		frame.fin = header.fin;
		frame.mask = header.mask;
		memcpy(frame.mask_buf, maskingKey, 4);
		frame.opcode = header.opcode;
		frame.payload_length = extendedPayload;
		frame.payload = data;
		return frame;
	}

	bool connect(String host, int port) {
		bool connected = this->_client->connect(host, port);
		if (!connected) return false;

		// TODO fix hardcoded Key
		String handshake = "GET / HTTP/1.1\r\n";
		handshake += "Upgrade: websocket\r\n";
		handshake += "Connection: Upgrade\r\n";
		handshake += "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";
		handshake += "Sec-WebSocket-Version: 13\r\n";
		handshake += "\r\n";

		this->_client->send(handshake);
		// TODO don't ignore server's response
		while (true) {
			auto line = this->_client->readLine();
			std::cout << "Got: " << line << std::endl;
			if (line == "\r\n") break;
		}

		auto frame = recvFrame();
		frame.log();
	}

	void poll() {
		auto frame = recvFrame();
		frame.log();
	}

	bool available() {
		return this->_client->available();
	}

	~WebSocketsClient() {
		delete this->_client;
	}
private:
	TcpClient* _client;
};


// **** Windows Stuff!!! ****

#define WIN32_LEAN_AND_MEAN

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

SOCKET windowsTcpConnect(String host, int port) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return INVALID_SOCKET;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return INVALID_SOCKET;
	}

	return ConnectSocket;
}


bool windowsTcpSend(uint8_t* buffer, uint32_t len, SOCKET socket) {
	// Send an initial buffer
	const char* cBuffer = (const char*)buffer;

	int iResult = send(socket, cBuffer, len, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return false;
	}

	printf("Bytes Sent: %ld\n", iResult);
	return true;
}

/*

// shutdown the connection since no more data will be sent
iResult = shutdown(ConnectSocket, SD_SEND);
if (iResult == SOCKET_ERROR) {
printf("shutdown failed with error: %d\n", WSAGetLastError());
closesocket(ConnectSocket);
WSACleanup();
return 1;
}


*/
void windowsTcpRecive(uint8_t* buffer, uint32_t len, SOCKET socket) {
	// Receive until the peer closes the connection
	int iResult = recv(socket, (char*)buffer, len, 0);
	if (iResult > 0) {
		return;
		printf("Bytes received: %d\n", iResult);
	}
	else if (iResult == 0) {
		printf("Connection closed\n");
	}
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
	}
}

class WinTcpClient : public TcpClient {
public:
	virtual bool connect(String host, int port) {
		this->socket = windowsTcpConnect(host, port);
		return available();
	}
	virtual bool available() {
		return socket != INVALID_SOCKET;
	}
	virtual void send(String data) {
		windowsTcpSend((uint8_t*) data.c_str(), data.size(), this->socket);
	}
	virtual void send(uint8_t* data, uint32_t len) {
		windowsTcpSend(data, len, this->socket);
	}
	virtual String readLine() {
		uint8_t byte = '0';
		String line;
		windowsTcpRecive(&byte, 1, this->socket);
		while (true) {
			line += (char)byte;
			if (byte == '\n') break;
			windowsTcpRecive(&byte, 1, this->socket);
		}
		return line;
	}
	virtual void read(uint8_t* buffer, uint32_t len) {
		windowsTcpRecive(buffer, len, this->socket);
	}
	~WinTcpClient() {
		closesocket(socket);
		WSACleanup();
	}

private:
	SOCKET socket;
};

int main() {
	WebSocketsClient client(new WinTcpClient);
	client.connect("localhost", 8080);
	client.poll();
	client.poll();

	system("pause");
}