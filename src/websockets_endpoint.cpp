#include "websockets/websockets_endpoint.h"
#include <memory.h>

WebSocketsEndpoint::WebSocketsEndpoint(TcpSocket& socket) : _socket(socket) {
    // Empty
}

bool WebSocketsEndpoint::poll() {
    return this->_socket.poll();
}

Header readHeaderFromSocket(TcpSocket& socket) {
    Header header;
    header.payload = 0;
    socket.read((uint8_t*)&header, 2);
    return std::move(header);
}

uint64_t readExtendedPayloadLength(TcpSocket& socket, const Header& header) {
    uint64_t extendedPayload = header.payload;
    // in case of extended payload length
    if (header.payload == 126) {
        // read next 16 bits as payload length
        uint16_t tmp = 0;
        socket.read((uint8_t*)&tmp, 2);
        tmp = (tmp << 8) | (tmp >> 8);
        extendedPayload = tmp;
    }
    else if (header.payload == 127) {
        // TODO: read next 64 bits as payload length and handle such very long messages
    }

    return extendedPayload;
}

void readMaskingKey(TcpSocket& socket, uint8_t* outputBuffer) {
    socket.read((uint8_t*)outputBuffer, 4);
}

String readData(TcpSocket& socket, uint64_t extendedPayload) {
    const uint64_t BUFFER_SIZE = 64;

    String data = "";
    uint8_t buffer[BUFFER_SIZE];
    uint64_t done_reading = 0;
    while (done_reading < extendedPayload) {
        uint64_t to_read = extendedPayload - done_reading >= BUFFER_SIZE ? BUFFER_SIZE : extendedPayload - done_reading;
        socket.read(buffer, to_read);
        done_reading += to_read;

        for (int i = 0; i < to_read; i++) {
            data += (char)buffer[i];
        }
    }
    return data;
}

String remaskData(String& data, const uint8_t* const maskingKey, uint64_t payloadLength) {
    for (int i = 0; i < payloadLength; i++) {
        data[i] = data[i] ^ maskingKey[i % 4];
    }
}

WebsocketsFrame WebSocketsEndpoint::recv() {
    auto header = readHeaderFromSocket(this->_socket);
    uint64_t payloadLength = readExtendedPayloadLength(this->_socket, header);
    
    uint8_t maskingKey[4];
    // if masking is set
    if (header.mask) {
        readMaskingKey(this->_socket, maskingKey);
    }

    // read the message's payload (data) according to the read length
    String data = readData(this->_socket, payloadLength);

    // if masking is set un-mask the message
    if (header.mask) {
        remaskData(data, maskingKey, payloadLength);
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

void WebSocketsEndpoint::send(String data, uint8_t opcode, bool mask, uint8_t maskingKey[4]) {
    Header header;
    header.fin = 1;
    header.flags = 0b000;
    header.opcode = opcode;
    header.mask = mask? 1: 0;
    header.payload = data.size() < 126? data.size(): data.size() > 1<<16? 127: 126;

    //header.log();
    // send initial header
    this->_socket.send((uint8_t*) &header, 2);

    if(header.payload == 126) {
        // send 16 bit length
        uint16_t extendedLen = data.size();
        // swap the bytes
        extendedLen = extendedLen << 8 | extendedLen >> 8;

        this->_socket.send((uint8_t*) &extendedLen, 2);
    } else if(header.payload == 127) {
        // TODO handle very long messages
    }

    // if masking is set, send the masking key
    if(mask) {
        this->_socket.send((uint8_t*) &maskingKey, 4);
    }

    this->_socket.send(data);
}

void WebSocketsEndpoint::close() {
    send("", MessageType::Close);
    this->_socket.close();
}

void WebSocketsEndpoint::pong(String msg) {
    send(msg, MessageType::Ping);
}

void WebSocketsEndpoint::ping(String msg) {
    send(msg, MessageType::Pong);
}

WebSocketsEndpoint::~WebSocketsEndpoint() {}