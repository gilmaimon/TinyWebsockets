#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

volatile bool didClientCloseConnection = false;

void test_server() {
    WebsocketsServer server;
    server.listen(8181);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );

    connectedClient.onMessage([](WebsocketsClient& client, WebsocketsMessage msg) {
        std::cout << "Got Message `" << msg.data() << "`, sending echo" << std::endl;
        if(msg.isText()) {
            client.send("Echo: " + msg.data());
        }
    });

    connectedClient.onEvent([](WebsocketsClient& client, WebsocketsEvent event, std::string data) {
        std::cout << "Event" << std::endl;
        if(event == WebsocketsEvent::ConnectionClosed) {
            std::cout << "Conn Closed" << std::endl;
        }
    });

    while(connectedClient.available()) {
        connectedClient.poll();
    }
    std::cout << "After Loop" << std::endl;
    std::cout << "Setting Flag" << std::endl;
    didClientCloseConnection = true;
}

TEST_CASE( "Testing Server creation" ) {
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

    std::cout << "Calling Close" << std::endl;
    client.close();
    std::cout << "Joining" << std::endl;
    serverThread.join();

    std::cout << "Testing" << std::endl;
    REQUIRE ( didClientCloseConnection == true );
}