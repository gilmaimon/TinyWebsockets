#define CATCH_CONFIG_MAIN

#define SOCKET int

#include "catch.hpp"
#include <websockets/websockets_client.h>
#include <network/windows/win_tcp_client.h>

using namespace websockets;
using namespace network;

#define PING_DATA "Hi, This is data sent over a PING"

TEST_CASE( "Testing Existing Server Connection" ) {
	auto client = WebsocketsClient::Create<WinTcpClient>();
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onEvent([&done](WebsocketsEvent event, auto data) {
        if(event == WebsocketsEvent::GotPong) {
            REQUIRE( data == PING_DATA );
            done = true;
        }
    });

    REQUIRE( client.ping(PING_DATA) == true);
    while(!done) client.poll();
}