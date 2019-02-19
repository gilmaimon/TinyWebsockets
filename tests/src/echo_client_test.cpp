#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define SOCKET int

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/network/windows/win_tcp_client.hpp>

using namespace websockets;
using namespace network;

TEST_CASE( "Testing Existing Server Connection - Send and recv" ) {
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