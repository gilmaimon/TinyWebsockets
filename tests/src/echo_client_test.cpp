#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

TEST_CASE( "Testing Existing Server Connection - Send and recv" ) {
	WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onMessage([&done](auto message){
        REQUIRE( message.data() == "ECHO: Hello Server" );
        done = true;
    });

    REQUIRE( client.send("Hello Server") == true);
    while(!done) client.poll();
}