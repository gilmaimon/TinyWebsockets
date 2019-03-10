#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

TEST_CASE( "Localhost Server, Testing Error Code - 1001" ) {    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") );

    REQUIRE( client.send("close-me-1") );

    std::this_thread::sleep_for (std::chrono::seconds(1));

    client.poll();

    REQUIRE ( client.available() == false );
    REQUIRE ( client.getCloseReason() == CloseReason_GoingAway );
}

TEST_CASE( "Localhost Server, Testing Error Code - 1002" ) {    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") );

    REQUIRE( client.send("close-me-2") );

    std::this_thread::sleep_for (std::chrono::seconds(1));

    client.poll();

    REQUIRE ( client.available() == false );
    REQUIRE ( client.getCloseReason() == CloseReason_ProtocolError );
}

TEST_CASE( "Localhost Server, Testing Error Code - 1003" ) {    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") );

    REQUIRE( client.send("close-me-3") );

    std::this_thread::sleep_for (std::chrono::seconds(1));

    client.poll();

    REQUIRE ( client.available() == false );
    REQUIRE ( client.getCloseReason() == CloseReason_UnsupportedData );
}

TEST_CASE( "Localhost Server, Initiating Close with code" ) {    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") );

    client.close(CloseReason_PolicyViolation);

    std::this_thread::sleep_for (std::chrono::seconds(1));

    client.poll();

    REQUIRE ( client.available() == false );
    REQUIRE ( client.getCloseReason() == CloseReason_PolicyViolation );
}