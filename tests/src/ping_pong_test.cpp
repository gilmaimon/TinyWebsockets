#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

#define PING_DATA std::string("Hi, This is data sent over a PING")

TEST_CASE( "Testing Existing Server Connection - Ping pong test" ) {
	WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onEvent([&done](WebsocketsClient& client, WebsocketsEvent event, auto data) {
        if(event == WebsocketsEvent::GotPong) {
            REQUIRE( data == PING_DATA );
            done = true;
        }
    });

    REQUIRE( client.ping(PING_DATA) == true);
    while(!done) client.poll();
}
