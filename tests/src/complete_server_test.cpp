#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

void test_server() {
    int eventIdx = 0;
    WebsocketsServer server;
    server.listen(8186);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );

    connectedClient.onEvent([&](WebsocketsClient&, WebsocketsEvent event, std::string data){
        switch(eventIdx) {
            case 0:
                REQUIRE( event == WebsocketsEvent::GotPong );
                REQUIRE( data == "Ping Data" );
                break;

            case 1:
                REQUIRE( event == WebsocketsEvent::ConnectionClosed );
                REQUIRE( connectedClient.getCloseReason() == CloseReason_NormalClosure );
                break;

            default:
                REQUIRE( false == true ); // SHOULD not occure
                break;
        }
        ++eventIdx;
    });

    connectedClient.send("Hello Client");
    connectedClient.ping("Ping Data");
    
    while(connectedClient.available()) {
        connectedClient.poll();
    }

    REQUIRE ( eventIdx == 2 );
}

TEST_CASE( "Testing Complete client/server handling - complete callback" ) {
    std::thread serverThread(test_server);
    int eventIdx = 0;

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;

    client.onMessage([](WebsocketsClient&, WebsocketsMessage msg){
        REQUIRE( msg.isText() == true );
        REQUIRE( msg.data() == "Hello Client" );
    });

    client.onEvent([&](WebsocketsClient&, WebsocketsEvent event, std::string data){
        switch(eventIdx) {
            case 0:
                REQUIRE( event == WebsocketsEvent::ConnectionOpened );
                REQUIRE( data == "" );
                break;

            case 1:
                REQUIRE( event == WebsocketsEvent::GotPing );
                REQUIRE( data == "Ping Data" );
                break;

            case 2:
                REQUIRE( event == WebsocketsEvent::ConnectionClosed );
                REQUIRE( client.getCloseReason() == CloseReason_NormalClosure );
                break;

            default:
                REQUIRE( false == true ); // SHOULD not occure
                break;
        }
        ++eventIdx;
    });

    REQUIRE( client.connect("localhost", 8186, "/") );

    std::this_thread::sleep_for (std::chrono::seconds(1));
    client.poll();
        
    client.close();
    serverThread.join();

    REQUIRE( eventIdx == 3 );
}

TEST_CASE( "Testing Complete client/server handling - partial callbacks" ) {
    std::thread serverThread(test_server);
    int eventIdx = 0;

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;

    client.onMessage([](WebsocketsMessage msg){
        REQUIRE( msg.isText() == true );
        REQUIRE( msg.data() == "Hello Client" );
    });

    client.onEvent([&](WebsocketsEvent event, std::string data){
        switch(eventIdx) {
            case 0:
                REQUIRE( event == WebsocketsEvent::ConnectionOpened );
                REQUIRE( data == "" );
                break;

            case 1:
                REQUIRE( event == WebsocketsEvent::GotPing );
                REQUIRE( data == "Ping Data" );
                break;

            case 2:
                REQUIRE( event == WebsocketsEvent::ConnectionClosed );
                REQUIRE( client.getCloseReason() == CloseReason_NormalClosure );
                break;

            default:
                REQUIRE( false == true ); // SHOULD not occure
                break;
        }
        ++eventIdx;
    });

    REQUIRE( client.connect("localhost", 8186, "/") );

    std::this_thread::sleep_for (std::chrono::seconds(1));
    client.poll();
        
    client.close();
    serverThread.join();

    REQUIRE( eventIdx == 3 );
}