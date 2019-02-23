#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

TEST_CASE( "Benchmarking message sending to server" ) {
	WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onMessage([&done](auto message){
        REQUIRE( message.data() == "ECHO: Hello Server" );
        done = true;
    });

    BENCHMARK( "Sending Messages" ) {
        client.send("Hello Server");
    }

    //while(!done) client.poll();
    client.close();
}