#include "windows/win_tcp_client.h"

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

	//printf("Bytes Sent: %ld\n", iResult);
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

bool WinTcpSocket::connect(String host, int port) {
    this->socket = windowsTcpConnect(host, port);
    return available();
}
bool WinTcpSocket::poll() {
	unsigned long l;
	ioctlsocket(this->socket, FIONREAD, &l);
	return l > 0;
}
bool WinTcpSocket::available() {
    return socket != INVALID_SOCKET;
}
void WinTcpSocket::send(String data) {
    windowsTcpSend((uint8_t*) data.c_str(), data.size(), this->socket);
}
void WinTcpSocket::send(uint8_t* data, uint32_t len) {
    windowsTcpSend(data, len, this->socket);
}
String WinTcpSocket::readLine() {
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
void WinTcpSocket::read(uint8_t* buffer, uint32_t len) {
    windowsTcpRecive(buffer, len, this->socket);
}

void WinTcpSocket::close() {
	closesocket(socket);
    // TODO WSA cleanup shouldnt be called multiple times?
	WSACleanup();
}

WinTcpSocket::~WinTcpSocket() {
	close();
}