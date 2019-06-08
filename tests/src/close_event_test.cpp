#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

#define MSG_DATA std::string("Hi, This is data sent")

TEST_CASE( "Testing - Event callback for `close` should be called when user closes the connection (no-reason)" ) {
    WebsocketsClient client;
    volatile int closeCalled = 0;
    volatile int openCalled = 0;

    client.onEvent([&openCalled, &closeCalled](WebsocketsClient& client, WebsocketsEvent event, auto data) {
        if(event == WebsocketsEvent::ConnectionClosed) {
            REQUIRE( client.getCloseReason() == CloseReason_NormalClosure );
            ++closeCalled;
        } else if(event == WebsocketsEvent::ConnectionOpened) {
            REQUIRE( closeCalled == 0 );
            ++openCalled;
        } else {
            REQUIRE( false == true ); // Should never happen in this test
        }
    });

    REQUIRE( client.connect("localhost", 8080, "/") == true );
    REQUIRE( client.available() == true );
    client.close();
    
    REQUIRE( client.available() == false );
    REQUIRE( client.getCloseReason() == CloseReason_NormalClosure );
    REQUIRE( closeCalled == 1 );
    REQUIRE( openCalled == 1 );
}

TEST_CASE( "Testing - Event callback for `close` should be called when user closes the connection (specified-reason)" ) {
    WebsocketsClient client;
    volatile int closeCalled = 0;
    volatile int openCalled = 0;

    client.onEvent([&openCalled, &closeCalled](WebsocketsClient& client, WebsocketsEvent event, auto data) {
        if(event == WebsocketsEvent::ConnectionClosed) {
            REQUIRE( client.getCloseReason() == CloseReason_UnsupportedData );
            ++closeCalled;
        } else if(event == WebsocketsEvent::ConnectionOpened) {
            REQUIRE( closeCalled == 0 );
            ++openCalled;
        } else {
            REQUIRE( false == true ); // Should never happen in this test
        }
    });

    REQUIRE( client.connect("localhost", 8080, "/") == true );
    REQUIRE( client.available() == true );
    client.close(CloseReason_UnsupportedData);
    
    REQUIRE( client.available() == false );
    REQUIRE( client.getCloseReason() == CloseReason_UnsupportedData );
    REQUIRE( closeCalled == 1 );
    REQUIRE( openCalled == 1 );
}

TEST_CASE( "Testing - Event callback for `close` should be called when abnormal closure occurs" ) {
    std::shared_ptr<network::TcpClient> intenalTcpClient = std::make_shared<WSDefaultTcpClient>();
    WebsocketsClient client(intenalTcpClient);

    volatile int closeCalled = 0;
    volatile int openCalled = 0;

    client.onEvent([&openCalled, &closeCalled](WebsocketsClient& client, WebsocketsEvent event, auto data) {
        if(event == WebsocketsEvent::ConnectionClosed) {
            REQUIRE( client.getCloseReason() == CloseReason_AbnormalClosure );
            ++closeCalled;
        } else if(event == WebsocketsEvent::ConnectionOpened) {
            REQUIRE( closeCalled == 0 );
            ++openCalled;
        } else {
            REQUIRE( false == true ); // Should never happen in this test
        }
    });

    REQUIRE( client.connect("localhost", 8080, "/") == true );
    REQUIRE( client.available() == true );

    // abnormal tcp closure
    intenalTcpClient->close();
    
    REQUIRE( client.available() == false );
    REQUIRE( client.getCloseReason() == CloseReason_AbnormalClosure );
    REQUIRE( closeCalled == 1 );
    REQUIRE( openCalled == 1 );
}
