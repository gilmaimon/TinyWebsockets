#pragma once

#include "common.h"
#include "network/tcp_client.h"
#include "websockets/data_frame.h"
#include "websockets/message.h"

namespace websockets::internals {
    class WebSocketsEndpoint {
    public:
        WebSocketsEndpoint(network::TcpSocket& socket);
        bool poll();
        WebsocketsFrame recv();
        void send(String data, uint8_t opcode, bool mask = false, uint8_t maskingKey[4] = nullptr);    
        
        void ping(String msg = "");
        void pong(String msg = "");
        void close();
        virtual ~WebSocketsEndpoint();
    private:
        network::TcpSocket& _socket;
    };
}