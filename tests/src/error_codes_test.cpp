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

void test_server_expects_normal_close() {
    WebsocketsServer server;
    server.listen(8189);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );
    while(connectedClient.available()) connectedClient.poll();

    REQUIRE ( connectedClient.getCloseReason() == CloseReason_NormalClosure );
}

TEST_CASE( "In-Program server, testing for default close() code" ) { 
    std::thread serverThread(test_server_expects_normal_close);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8189, "/") );
    REQUIRE( client.available() );

    client.close();
    serverThread.join();
}

void test_server_expects_going_away() {
    WebsocketsServer server;
    server.listen(8189);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );
    while(connectedClient.available()) connectedClient.poll();

    REQUIRE ( connectedClient.getCloseReason() == CloseReason_GoingAway );
}

TEST_CASE( "In-Program server, testing for default dtor code" ) { 
    std::thread serverThread(test_server_expects_going_away);

    std::this_thread::sleep_for (std::chrono::seconds(1));

    {
        WebsocketsClient client;
        REQUIRE( client.connect("localhost", 8189, "/") );
        REQUIRE( client.available() );
    } // At this point the d'tor of client will be called

    serverThread.join();
}