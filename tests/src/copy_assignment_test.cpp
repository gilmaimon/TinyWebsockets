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
  server.listen(8282);

  auto connectedClient = server.accept();
  REQUIRE( connectedClient.available() );

  connectedClient.onMessage([&](WebsocketsMessage msg) {
    connectedClient.send(msg.data());
  });

  while(connectedClient.available()) {
    connectedClient.poll();
  } 
  connectedClient.close();
}

TEST_CASE( "Testing Server Creation and basic Communication" ) {
    std::thread serverThread(test_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8282, "/") );
    
    client.send("Hello Server");
    auto message = client.readBlocking();

    REQUIRE( message.isText() );
    REQUIRE( message.data() == "Hello Server" );

    {
      auto client2 = client;
      client2.send("Hi");
      auto msg = client2.readBlocking();
      REQUIRE( msg.isText() );
      REQUIRE( msg.data() == "Hi" );
    }

    REQUIRE( client.available() == false );

    client.close();
    serverThread.join();
}