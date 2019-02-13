#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/message.h"
#include "websockets/websockets_client.h"
#include "crypto/crypto.h"
#include <memory.h>

WebSocketsClient::WebSocketsClient(TcpClient* client) : _client(client), WebSocketsEndpoint(*client), _connectionOpen(false) {
    // Empty
}

struct HandshakeRequestResult {
    String requestStr;
    String expectedAcceptKey;
};
HandshakeRequestResult generateHandshake(String uri) {
    String randomBytes = crypto::randomBytes(16);

    String handshake = "GET " + uri + " HTTP/1.1\r\n";
    handshake += "Upgrade: websocket\r\n";
    handshake += "Connection: Upgrade\r\n";
    handshake += "Sec-WebSocket-Key: " + crypto::base64Encode(randomBytes) + "\r\n";
    handshake += "Sec-WebSocket-Version: 13\r\n";
    handshake += "\r\n";

    String expectedAccept = crypto::base64Encode(
        crypto::sha1(
            crypto::base64Encode(randomBytes) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
        )
    );

    HandshakeRequestResult result;
    result.requestStr = handshake;
    result.expectedAcceptKey = expectedAccept;
    return std::move(result);
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

struct HandshakeResponseResult {
    bool isSuccess;
    String serverAccept;
};
HandshakeResponseResult parseHandshakeResponse(String responseHeaders) {
    bool didUpgradeToWebsockets = false, isConnectionUpgraded = false;
    String serverAccept = "";
    size_t idx = 0;
    while(idx < responseHeaders.size()) {
        String key = "", value = "";
        // read header key
        while(idx < responseHeaders.size() && responseHeaders[idx] != ':') key += responseHeaders[idx++];

        // ignore ':' and whitespace
        ++idx;
        while(idx < responseHeaders.size() && isWhitespace(responseHeaders[idx])) idx++;

        // read header value until \r\n or whitespace
        while(idx < responseHeaders.size() && !isWhitespace(responseHeaders[idx])) value += responseHeaders[idx++];

        // ignore rest of whitespace
        while(idx < responseHeaders.size() && isWhitespace(responseHeaders[idx])) idx++;

        if(key == "Upgrade") {
            didUpgradeToWebsockets = (value == "websocket");
        } else if(key == "Connection") {
            isConnectionUpgraded = (value == "Upgrade");
        } else if(key == "Sec-WebSocket-Accept") {
            serverAccept = value;
        }
    }

    HandshakeResponseResult result;
    result.isSuccess = serverAccept != "" && didUpgradeToWebsockets && isConnectionUpgraded;
    result.serverAccept = serverAccept;
    return result;
}

bool WebSocketsClient::connect(String host, int port) {
    this->_connectionOpen = this->_client->connect(host, port);
    if (!this->_connectionOpen) return false;

    auto handshake = generateHandshake("/");
    std::cout << "Sending handshake" << std::endl;
    std::cout << handshake.requestStr;
    this->_client->send(handshake.requestStr);



    auto head = this->_client->readLine();
    if(head != "HTTP/1.1 101 Switching Protocols\r\n") {
        closeConnection();
        return false;
    }

    String serverResponseHeaders = "";
    String line = "";
    while (true) {
        line = this->_client->readLine();
        serverResponseHeaders += line;
        if (line == "\r\n") break;
    }
    
    auto parsedResponse = parseHandshakeResponse(serverResponseHeaders);
    if(parsedResponse.isSuccess == false || parsedResponse.serverAccept != handshake.expectedAcceptKey) {
        closeConnection();
        return false;
    }

    return true;
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
    this->_connectionOpen |= this->_client->available();
    return _connectionOpen;
}


void WebSocketsClient::_handlePing(WebsocketsMessage) {
    // TODO handle ping
}

void WebSocketsClient::_handlePong(WebsocketsMessage) {
    // TODO handle pong
}

void WebSocketsClient::_handleClose(WebsocketsMessage) {
    this->_connectionOpen = false;
}

void WebSocketsClient::closeConnection() {
    WebSocketsEndpoint::close();
    this->_connectionOpen = false;
}

WebSocketsClient::~WebSocketsClient() {
    delete this->_client;
}