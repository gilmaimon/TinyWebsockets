#pragma once

#include "common.h"
#include "network/tcp_socket.h"

namespace websockets::network {
	struct TcpClient : public TcpSocket {
		virtual bool connect(WSString host, int port) = 0;
	};
}