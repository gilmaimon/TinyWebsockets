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
    server.listen(8184);

    auto connectedClient = server.accept();
    REQUIRE( connectedClient.available() );

    {
      auto msg = connectedClient.readBlocking();

      REQUIRE( msg.isBinary() == true );
      REQUIRE( msg.data().size() == 21);
      REQUIRE( msg.data() == std::string("Hello Server\xba\xbe\x00\xca\xfe-END", 21));
    }

    connectedClient.close();
}

TEST_CASE( "Testing Sending fragmented binary data (with null in the middle)" ) {
    std::thread serverThread(test_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8184, "/") );

    client.streamBinary("Hello Server");
    client.sendBinary("\xba\xbe\x00\xca\xfe", 5);
    client.end("-END");

    client.close();
    serverThread.join();
}