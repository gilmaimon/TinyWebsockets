#pragma once

#include "common.h"
#include <tcp_client.h>
#include "websockets/data_frame.h"

class WebSocketsEndpoint {
public:
    WebSocketsEndpoint(TcpSocket& socket);
    bool poll();
    WebsocketsFrame recv();
    void send(String data, uint8_t opcode, bool mask = false, uint8_t maskingKey[4] = nullptr);    
    virtual ~WebSocketsEndpoint();
private:
    TcpSocket& _socket;
};