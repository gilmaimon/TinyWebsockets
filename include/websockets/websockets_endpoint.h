#pragma once

#include "common.h"
#include <tcp_client.h>
#include "websockets/data_frame.h"
#include "websockets/message.h"

class WebSocketsEndpoint {
public:
    WebSocketsEndpoint(TcpSocket& socket);
    bool poll();
    WebsocketsFrame recv();
    void send(String data, uint8_t opcode, bool mask = false, uint8_t maskingKey[4] = nullptr);    
    
    void ping(String msg = "");
    void pong(String msg = "");
    void close();
    virtual ~WebSocketsEndpoint();
private:
    TcpSocket& _socket;
};