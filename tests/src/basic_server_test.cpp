#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

void test_server() {
    WebsocketsServer server;
    server.listen(8182);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );

    connectedClient.send("Hello Client");
    connectedClient.close();
}

TEST_CASE( "Testing Server Creation and basic Communication" ) {
    std::thread serverThread(test_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8182, "/") );

    auto message = client.readBlocking();

    REQUIRE( message.type() == MessageType::Text );
    REQUIRE( message.data() == "Hello Client" );
    
    client.close();
    serverThread.join();
}