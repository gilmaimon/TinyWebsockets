#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace websockets;

std::string generateLongMessage(size_t len) {
  std::string result;
  std::string vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  for(size_t i = 0; i < len; i++) {
    result += vocabulary[i % vocabulary.size()];
  }
  return result;
}

void test_echo_server() {
  WebsocketsServer server;
  server.listen(8183);

  auto connectedClient = server.accept();
  REQUIRE( connectedClient.available() );

  connectedClient.onMessage([](WebsocketsClient& client, WebsocketsMessage message){
    client.send(message.data());
  });

  while(connectedClient.available()) connectedClient.poll();
}

TEST_CASE( "In-Program Echo Server: Test sending and receiving very long messages (longer than 16 bits)" ) {
  std::thread serverThread(test_echo_server);

  std::cout << "In-Memory Server Test: " << std::endl;

  std::this_thread::sleep_for (std::chrono::seconds(1));
  
  WebsocketsClient client;
  REQUIRE( client.connect("localhost", 8183, "/") );

  // testing messages with longer payload (longer than 16 bits, sent as 64 bits according to the rfc)
  // testing messages up to 65536 * 10 in length (about half-million bytes in a single message)
  for(uint64_t dataLen = 65536; dataLen < 65536 * 10; dataLen += 65536) {
    std::cout << "Sending Message of len: " << dataLen << std::endl;
    auto longMessageData = generateLongMessage(dataLen);
    REQUIRE ( client.send(longMessageData) );

    auto message = client.readBlocking();
    REQUIRE( message.isText() );
    std::cout << " - Got in return message of len: " << message.data().size() << std::endl;
    REQUIRE( message.data() == longMessageData );
  }
  client.close();
  serverThread.join();
}

TEST_CASE( "Local Echo Server: Test sending and receiving very long messages (longer than 16 bits)" ) {  
  std::cout << std::endl << "Localhost Server Test: " << std::endl;
  WebsocketsClient client;
  REQUIRE( client.connect("localhost", 8080, "/") );

  // testing messages with longer payload (longer than 16 bits, sent as 64 bits according to the rfc)
  // testing messages up to 65536 * 10 in length (about half-million bytes in a single message)
  for(uint64_t dataLen = 65536; dataLen <= 65536 * 10; dataLen += 65536) {
    std::cout << "Sending Message of len: " << dataLen << std::endl;
    auto longMessageData = generateLongMessage(dataLen);
    REQUIRE ( client.send(longMessageData) );

    auto message = client.readBlocking();
    REQUIRE( message.isText() );
    std::cout << " - Got in return message of len: " << message.data().size() << std::endl;
    REQUIRE( message.data() == "ECHO: " + longMessageData );
  }
  client.close();
}