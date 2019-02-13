#pragma once

#include "common.h"
#include "network/tcp_socket.h"

namespace websockets::network {
	struct TcpClient : public TcpSocket {
		virtual bool connect(String host, int port) = 0;
	};
}