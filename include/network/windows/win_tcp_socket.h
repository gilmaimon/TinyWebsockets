#pragma once

#include "common.h"
#include "network/tcp_client.h"

#include <windows.h>

namespace websockets::network {
    class WinTcpSocket : public TcpSocket {
    public:
        bool connect(String host, int port);
        bool poll() override;
        bool available() override;
        void send(String data) override;
        void send(uint8_t* data, uint32_t len) override;
        String readLine() override;
        void read(uint8_t* buffer, uint32_t len) override;
        void close() override;
        virtual ~WinTcpSocket();

    private:
        SOCKET socket;
    };
}