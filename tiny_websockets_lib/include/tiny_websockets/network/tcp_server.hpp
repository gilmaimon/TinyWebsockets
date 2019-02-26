#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/network/tcp_socket.hpp>
#include <tiny_websockets/network/tcp_server.hpp>

namespace websockets { namespace network {
	struct TcpServer : public TcpSocket {
        virtual bool listen(WSString host, int port) = 0;
		virtual TcpServer* accept() = 0;
		virtual ~TcpServer() {}
	};
}} // websockets::network