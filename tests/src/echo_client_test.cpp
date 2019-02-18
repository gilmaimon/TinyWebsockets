#define CATCH_CONFIG_MAIN

#define SOCKET int

#include "catch.hpp"
#include <websockets/websockets_client.h>
#include <network/windows/win_tcp_client.h>

using namespace websockets;
using namespace network;

TEST_CASE( "Testing Existing Server Connection" ) {
	auto client = WebsocketsClient::Create<WinTcpClient>();
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onMessage([&done](auto message){
        REQUIRE( message.data() == "ECHO: Hello Server" );
        done = true;
    });

    REQUIRE( client.send("Hello Server") == true);
    while(!done) client.poll();
}