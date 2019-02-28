#include <tiny_websockets/internals/websockets_endpoint.hpp>
#include <iostream>

namespace websockets { namespace internals {
    WebsocketsEndpoint::WebsocketsEndpoint(network::TcpClient& client) : _client(client) {
        // Empty
    }

    bool WebsocketsEndpoint::poll() {
        return this->_client.poll();
    }

    Header readHeaderFromSocket(network::TcpClient& socket) {
        Header header;
        header.payload = 0;
        socket.read(reinterpret_cast<uint8_t*>(&header), 2);
        return header;
    }

    uint64_t readExtendedPayloadLength(network::TcpClient& socket, const Header& header) {
        uint64_t extendedPayload = header.payload;
        // in case of extended payload length
        if (header.payload == 126) {
            // read next 16 bits as payload length
            uint16_t tmp = 0;
            socket.read(reinterpret_cast<uint8_t*>(&tmp), 2);
            tmp = (tmp << 8) | (tmp >> 8);
            extendedPayload = tmp;
        }
        else if (header.payload == 127) {
            // TODO: read next 64 bits as payload length and handle such very long messages
        }

        return extendedPayload;
    }

    void readMaskingKey(network::TcpClient& socket, uint8_t* outputBuffer) {
        socket.read(reinterpret_cast<uint8_t*>(outputBuffer), 4);
    }

    WSString readData(network::TcpClient& socket, uint64_t extendedPayload) {
        const uint64_t BUFFER_SIZE = 64;

        WSString data = "";
        uint8_t buffer[BUFFER_SIZE];
        uint64_t done_reading = 0;
        while (done_reading < extendedPayload) {
            uint64_t to_read = extendedPayload - done_reading >= BUFFER_SIZE ? BUFFER_SIZE : extendedPayload - done_reading;
            socket.read(buffer, to_read);
            done_reading += to_read;

            for (uint64_t i = 0; i < to_read; i++) {
                data += static_cast<char>(buffer[i]);
            }
        }
        return data;
    }

    void remaskData(WSString& data, const uint8_t* const maskingKey, uint64_t payloadLength) {
        for (uint64_t i = 0; i < payloadLength; i++) {
            data[i] = data[i] ^ maskingKey[i % 4];
        }
    }

    WebsocketsFrame WebsocketsEndpoint::_recv() {
        auto header = readHeaderFromSocket(this->_client);
        uint64_t payloadLength = readExtendedPayloadLength(this->_client, header);

        uint8_t maskingKey[4];
        // if masking is set
        if (header.mask) {
            readMaskingKey(this->_client, maskingKey);
        }

        // read the message's payload (data) according to the read length
        WSString data = readData(this->_client, payloadLength);

        // if masking is set un-mask the message
        if (header.mask) {
            remaskData(data, maskingKey, payloadLength);
        }

        // Construct frame from data and header that was read
        WebsocketsFrame frame;
        frame.fin = header.fin;
        frame.mask = header.mask;

        frame.mask_buf[0] = maskingKey[0];
        frame.mask_buf[1] = maskingKey[1];
        frame.mask_buf[2] = maskingKey[2];
        frame.mask_buf[3] = maskingKey[3];

        frame.opcode = header.opcode;
        frame.payload_length = payloadLength;
        frame.payload = data;
        return frame;
    }

    WebsocketsMessage WebsocketsEndpoint::recv() {
        auto frame = _recv();
        auto msg = WebsocketsMessage::CreateFromFrame(std::move(frame));
        switch(msg.type()) {
            case MessageType::Binary:
                break; // Intentionally Empty
            
            case MessageType::Text: 
                break; // Intentionally Empty
            
            case MessageType::Ping:
                pong(internals::fromInterfaceString(msg.data()));
                break;

            case MessageType::Pong:
                break; // Intentionally Empty

            case MessageType::Close:
                close();
                break;
        }

        return std::move(msg);
    }

    bool WebsocketsEndpoint::send(WSString data, uint8_t opcode, bool mask, uint8_t maskingKey[4]) { 
        return send(reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str())), data.size(), opcode, mask, maskingKey);
    }

    bool WebsocketsEndpoint::send(uint8_t* data, size_t len, uint8_t opcode, bool mask, uint8_t maskingKey[4]) {
        HeaderWithExtended header;
        header.fin = 1;
        header.flags = 0;
        header.opcode = opcode;
        header.mask = mask? 1: 0;

        size_t headerLen = 2;

        if(len < 126) {
            header.payload = len;
        } else if(len < 65536) {
            header.payload = 126;
            header.extendedPayload = (len << 8) | (len >> 8);
            headerLen = 4; // with 2 bytes of extra length
        } else {
            // TODO properly handle very long message
            // ?? header.extraExtenedePayload;
            header.payload = 127;
        }
        
        // send header
        this->_client.send(reinterpret_cast<uint8_t*>(&header), headerLen);

        // if masking is set, send the masking key
        if(mask) {
            this->_client.send(reinterpret_cast<uint8_t*>(maskingKey), 4);
        }

        this->_client.send(data, len);
        return true; // TODO dont assume success
    }

    void WebsocketsEndpoint::close() {
        std::cout << " - someone called WebsocketsEndpoint::close" << std::endl;
        if(this->_client.available()) {
            std::cout << " - is available, sending close message: " << std::endl;
            send("", MessageType::Close);
            std::cout << " - sent, closing tcp socket. " << std::endl;
            this->_client.close();
        }
    }

    bool WebsocketsEndpoint::pong(WSString msg) {
        // Pong data must be shorter than 125 bytes
        if(msg.size() > 125)  {
            return false;
        }
        else {
            return send(msg, MessageType::Pong);
        }
    }

    bool WebsocketsEndpoint::ping(WSString msg) {
        // Ping data must be shorter than 125 bytes
        if(msg.size() > 125) {
            return false;
        }
        else {
            return send(msg, MessageType::Ping);
        }
    }

    WebsocketsEndpoint::~WebsocketsEndpoint() {}
}} // websockets::internals
