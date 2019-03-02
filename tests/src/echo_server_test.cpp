#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

volatile bool didClientCloseConnection = false;
volatile bool didGotCloseMessage = false;

void test_server() {
    WebsocketsServer server;
    server.listen(8181);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );

    connectedClient.onMessage([](WebsocketsClient& client, WebsocketsMessage msg) {
        if(msg.isText()) {
            client.send("Echo: " + msg.data());
        }
    });

    connectedClient.onEvent([](WebsocketsClient& client, WebsocketsEvent event, std::string data) {
        if(event == WebsocketsEvent::ConnectionClosed) {
            didGotCloseMessage = true;
        }
    });

    while(connectedClient.available()) {
        connectedClient.poll();
    }

    didClientCloseConnection = true;
}

TEST_CASE( "Testing An Echo Server Demo with messages longer than 126 bytes" ) {
    std::thread serverThread(test_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("ws://localhost:8181") );
    
    // a test message with content "Hello", server should return echo
    {
        std::string shortMessage = "Hello World";
        REQUIRE( client.send(shortMessage) );
        auto message = client.readBlocking();
        REQUIRE( message.type() == MessageType::Text );
        REQUIRE( message.data() == "Echo: " + shortMessage );
    }   
    
    // a test message with content "World", server should return echo
    {
        std::string longMessage = "Hello World. This is a message that is longer than 126 bytes and so it will be sent diffrently than shorter message. This message is 148 bytes long.";
        REQUIRE( client.send(longMessage) );
        auto message = client.readBlocking();
        REQUIRE( message.type() == MessageType::Text );
        REQUIRE( message.data() == "Echo: " + longMessage );
    }

    client.close();
    serverThread.join();

    REQUIRE ( (didClientCloseConnection && didGotCloseMessage) );
}