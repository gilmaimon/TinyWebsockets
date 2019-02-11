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

WebsocketsFrame WebSocketsClient::recvFrame() {
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