#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/network/tcp_server.hpp>
#include <tiny_websockets/internals/data_frame.hpp>
#include <functional>

namespace websockets {
	class WebsocketsServer {
	public:
		WebsocketsServer(network::TcpServer* server = new WSDefaultTcpServer);
		
		WebsocketsServer(const WebsocketsServer& other) = delete;
		WebsocketsServer(const WebsocketsServer&& other) = delete;
		
		WebsocketsServer& operator=(const WebsocketsServer& other) = delete;
		WebsocketsServer& operator=(const WebsocketsServer&& other) = delete;

        void listen(WSInterfaceString host, int port);

		virtual ~WebsocketsServer();

	private:
		network::TcpServer* _server;
	};
}