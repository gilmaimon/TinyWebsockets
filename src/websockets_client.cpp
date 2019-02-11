#include "common.h"
#include "tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/websockets_client.h"
#include <memory.h>

WebSocketsClient::WebSocketsClient(TcpClient* client) : _client(client) {
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

    auto frame = recvFrame();
    frame.log();
}

void WebSocketsClient::poll() {
    auto frame = recvFrame();
    frame.log();
}

bool WebSocketsClient::available() {
    return this->_client->available();
}

WebSocketsClient::~WebSocketsClient() {
    delete this->_client;
}

Header readHeaderFromSocket(TcpClient* client) {
    Header header;
    header.payload = 0;
    client->read((uint8_t*)&header, 2);
    return std::move(header);
}

uint64_t readExtendedPayloadLength(TcpClient* client, const Header& header) {
    uint64_t extendedPayload = header.payload;
    // in case of extended payload length
    if (header.payload == 126) {
        // read next 16 bits as payload length
        uint16_t tmp = 0;
        client->read((uint8_t*)&tmp, 2);
        tmp = (tmp << 8) | (tmp >> 8);
        extendedPayload = tmp;
    }
    else if (header.payload == 127) {
        // TODO: read next 64 bits as payload length and handle such very long messages
    }

    return extendedPayload;
}

void readMaskingKey(TcpClient* client, uint8_t* outputBuffer) {
    client->read((uint8_t*)outputBuffer, 4);
}

String readData(TcpClient* client, uint64_t extendedPayload) {
    const uint64_t BUFFER_SIZE = 64;

    String data = "";
    uint8_t buffer[BUFFER_SIZE];
    uint64_t done_reading = 0;
    while (done_reading < extendedPayload) {
        uint64_t to_read = extendedPayload - done_reading >= BUFFER_SIZE ? BUFFER_SIZE : extendedPayload - done_reading;
        client->read(buffer, to_read);
        done_reading += to_read;

        for (int i = 0; i < to_read; i++) {
            data += (char)buffer[i];
        }
    }
    return data;
}

String unmaskData(String& data, const uint8_t* const maskingKey, uint64_t payloadLength) {
    for (int i = 0; i < payloadLength; i++) {
        data[i] = data[i] ^ maskingKey[i % 4];
    }
}

WebsocketsFrame WebSocketsClient::recvFrame() {
    auto header = readHeaderFromSocket(this->_client);
    uint64_t payloadLength = readExtendedPayloadLength(this->_client, header);
    
    uint8_t maskingKey[4];
    // if masking is set
    if (header.mask) {
        readMaskingKey(this->_client, maskingKey);
    }

    // read the message's payload (data) according to the read length
    String data = readData(this->_client, payloadLength);

    // if masking is set un-mask the message
    if (header.mask) {
        unmaskData(data, maskingKey, payloadLength);
    }

    // Construct frame from data and header that was read
    WebsocketsFrame frame;
    frame.fin = header.fin;
    frame.mask = header.mask;
    memcpy(frame.mask_buf, maskingKey, 4);
    frame.opcode = header.opcode;
    frame.payload_length = payloadLength;
    frame.payload = data;
    return frame;
}