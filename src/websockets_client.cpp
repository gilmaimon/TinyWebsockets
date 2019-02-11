#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/websockets_client.h"
#include <memory.h>

WebSocketsClient::WebSocketsClient(TcpClient* client) : _client(client), WebSocketsEndpoint(*client) {
    // Empty
}

bool WebSocketsClient::connect(String host, int port) {
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
}

void WebSocketsClient::poll() {
    auto frame = WebSocketsEndpoint::recv();
    frame.log();
}

void WebSocketsClient::send(String data) {
    WebSocketsEndpoint::send(data, 0x1);
}

bool WebSocketsClient::available() {
    return this->_client->available();
}

WebSocketsClient::~WebSocketsClient() {
    delete this->_client;
}