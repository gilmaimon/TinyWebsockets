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

void test_echo_server() {
    WebsocketsServer server;
    server.listen(8182);

    volatile int messagesReceived = 0;

    auto connectedClient = server.accept();
    connectedClient.onMessage([&](WebsocketsClient&, WebsocketsMessage msg){
        if(msg.isText()) {
            connectedClient.send(msg.data());
            messagesReceived++;
        } else if(msg.isBinary()) {
            connectedClient.sendBinary(msg.data());
            messagesReceived++;
        }
    });
    
    while(connectedClient.available()) {
        connectedClient.poll();
    }

    REQUIRE(messagesReceived == 2);
}

TEST_CASE( "Testing Server Creation and basic Communication" ) {
    std::thread serverThread(test_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8182, "/") );

    auto message = client.readBlocking();

    REQUIRE( message.isText() );
    REQUIRE( message.data() == "Hello Client" );
    
    client.close();
    serverThread.join();
}

TEST_CASE("Testing Existing Server Connection - sending empty messages") {
  std::thread serverThread(test_echo_server);
  WebsocketsClient client;
  REQUIRE(client.connect("localhost", 8182, "/") == true);
  volatile bool done = false;
  int stage = 0;

  client.onMessage([&done, &stage](WebsocketsClient& client, auto message) {
    if (stage == 0) {
      REQUIRE(message.data() == "");
      REQUIRE(message.isText() == true);
      REQUIRE(message.data().size() == 0);
      stage = 1;
    }
    else if (stage == 1) {
      REQUIRE(message.data() == "");
      REQUIRE(message.isBinary() == true);
      REQUIRE(message.data().size() == 0);
      done = true;
    }
  });

  REQUIRE(client.send("") == true);
  REQUIRE(client.sendBinary("") == true);

  while (!done) client.poll();
  client.close();
  serverThread.join();
}

TEST_CASE("Testing Remote Server Connection - sending empty messages") {
  WebsocketsClient client;
  REQUIRE(client.connect("localhost", 9999, "/") == true);
  volatile bool done = false;
  int stage = 0;

  client.onMessage([&done, &stage](WebsocketsClient& client, auto message) {
    if (stage == 0) {
      REQUIRE(message.data() == "");
      REQUIRE(message.isText() == true);
      REQUIRE(message.data().size() == 0);
      stage = 1;
    }
    else if (stage == 1) {
      REQUIRE(message.data() == "");
      REQUIRE(message.isBinary() == true);
      REQUIRE(message.data().size() == 0);
      done = true;
    }
  });

  REQUIRE(client.send("") == true);
  REQUIRE(client.sendBinary("") == true);

  while (!done) client.poll();
  client.close();
}