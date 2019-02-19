#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define SOCKET int

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/network/windows/win_tcp_client.hpp>


using namespace websockets;
using namespace network;

#define PING_DATA "Hi, This is data sent over a PING"

TEST_CASE( "Testing Existing Server Connection - Ping pong test" ) {
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