#pragma once

#include <tiny_websockets/ws_config_defs.hpp>
#include <string>

namespace websockets {
    typedef std::string WSString;
}

#ifdef _WIN32
    #include <tiny_websockets/network/windows/win_tcp_client.hpp>
    #include <tiny_websockets/network/windows/win_tcp_socket.hpp>
    #define DEFAULT_CLIENT websockets::network::WinTcpClient
    #define DEFAULT_SOCKET websockets::network::WinTcpSocket
#elif __linux__
    #include <tiny_websockets/network/linux/linux_tcp_client.hpp>
    #define DEFAULT_CLIENT websockets::network::LinuxTcpClient
    #define DEFAULT_SOCKET websokcets::network::LinuxTcpSocket
#endif
