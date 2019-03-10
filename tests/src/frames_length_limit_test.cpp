#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define _WS_CONFIG_MAX_MESSAGE_SIZE 1337

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>

#include <iostream>

using namespace websockets;

std::string generateLongMessage(size_t len) {
  std::string result;
  std::string vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  for(size_t i = 0; i < len; i++) {
    result += vocabulary[i % vocabulary.size()];
  }
  return result;
}

TEST_CASE( "Test sending message longer than the limit set" ) {
    WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") );

    REQUIRE ( client.send(generateLongMessage(1)) == true );
    REQUIRE ( client.send(generateLongMessage(10)) == true );
    REQUIRE ( client.send(generateLongMessage(100)) == true );
    
    REQUIRE ( client.send(generateLongMessage(_WS_CONFIG_MAX_MESSAGE_SIZE)) == true );
    
    REQUIRE ( client.available() == true );
    REQUIRE ( client.send(generateLongMessage(_WS_CONFIG_MAX_MESSAGE_SIZE + 1) ) == false );
    REQUIRE ( client.send(generateLongMessage(_WS_CONFIG_MAX_MESSAGE_SIZE * 2) ) == false );
    REQUIRE ( client.available() == true );
    
    client.close();
}