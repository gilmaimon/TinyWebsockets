#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/message.h"
#include "websockets/websockets_client.h"
#include "crypto/crypto.h"
#include <memory.h>

WebSocketsClient::WebSocketsClient(TcpClient* client) : _client(client), WebSocketsEndpoint(*client) {
    // Empty
}

struct HandshakeRequestResult {
    String requestStr;
    String expectedAcceptKey;
};
HandshakeRequestResult generateHandshake(String uri) {
    String randomBytes = "1234567890123456";

    String handshake = "GET " + uri + " HTTP/1.1\r\n";
    handshake += "Upgrade: websocket\r\n";
    handshake += "Connection: Upgrade\r\n";
    handshake += "Sec-WebSocket-Key: " + crypto::base64Encode(randomBytes) + "\r\n";
    handshake += "Sec-WebSocket-Version: 13\r\n";
    handshake += "\r\n";

    String expectedAccept = crypto::base64Encode(crypto::sha1(
        crypto::base64Encode(randomBytes) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    ));

    HandshakeRequestResult result;
    result.requestStr = handshake;
    result.expectedAcceptKey = expectedAccept;
    return std::move(result);
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t';
}

bool WebSocketsClient::connect(String host, int port) {
    bool connected = this->_client->connect(host, port);
    if (!connected) return false;

    auto handshake = generateHandshake("/");
    this->_client->send(handshake.requestStr);


    auto head = this->_client->readLine();
    if(head != "HTTP/1.1 101 Switching Protocols") {
        //TODO indicate Error!!
        return false;
    }

    // TODO don't ignore server's response
    while (true) {
        auto line = this->_client->readLine();
        std::cout << "Got: " << line << std::endl;
        if (line == "\r\n") break;
    }
}

void WebSocketsClient::onMessage(MessageCallback callback) {
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