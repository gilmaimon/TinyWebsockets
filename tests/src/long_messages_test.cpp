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

TEST_CASE( "Testing Server creation" ) {
    std::thread serverThread(test_echo_server);

    std::this_thread::sleep_for (std::chrono::seconds(1));
    
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8183, "/") );

    // Note: at the time of writing this test, only 2 bytes long (65536 in length) messages are supported
    for(size_t dataLen = 2; dataLen < 65536; dataLen += 100) {
      std::cout << "Sending Message of len: " << dataLen << std::endl;
      auto longMessageData = generateLongMessage(dataLen);
      REQUIRE ( client.send(longMessageData) );

      auto message = client.readBlocking();
      REQUIRE( message.type() == MessageType::Text );
      std::cout << " - Got in return message of len: " << message.data().size() << std::endl;
      REQUIRE( message.data() == longMessageData );
    }
    client.close();
    serverThread.join();
}