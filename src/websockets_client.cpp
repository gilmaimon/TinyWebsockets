#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/message.h"
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

void WebSocketsClient::setMessageHandler(MessageCallback callback) {
    this->_callback = callback;
}

void WebSocketsClient::poll() {
    if(!WebSocketsEndpoint::poll()) {
        return;
    }

    auto frame = WebSocketsEndpoint::recv();
    
    auto msg = WebsocketsMessage::CreateFromFrame(frame);
    if(msg.isBinary() || msg.isText()) {
        this->_callback(std::move(msg));
    } else if(msg.type() == MessageType::Ping) {
        _handlePing(std::move(msg));
    } else if(msg.type() == MessageType::Pong) {
        _handlePong(std::move(msg));
    } else if(msg.type() == MessageType::Close) {
        _handleClose(std::move(msg));
    }
}

void WebSocketsClient::send(String data) {
    WebSocketsEndpoint::send(data, MessageType::Text);
}

void WebSocketsClient::sendBinary(String data) {
    WebSocketsEndpoint::send(data, MessageType::Binary);
}

bool WebSocketsClient::available() {
    return this->_client->available();
}


void WebSocketsClient::_handlePing(WebsocketsMessage) {
    // TODO handle ping
}

void WebSocketsClient::_handlePong(WebsocketsMessage) {
    // TODO handle pong
}

void WebSocketsClient::_handleClose(WebsocketsMessage) {
    //TODO handle close
}

WebSocketsClient::~WebSocketsClient() {
    delete this->_client;
}