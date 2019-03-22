#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

TEST_CASE( "Testing SSL Client (with connect string https://)" ) {
  std::cout << "Test 1" << std::endl;
  WebsocketsClient client;
  REQUIRE( client.connect("https://echo.websocket.org/") == true );
  
  REQUIRE( client.send("Hello Server") == true);
  auto message = client.readBlocking();
  REQUIRE( message.data() == "Hello Server" );
  REQUIRE( client.available() == true );
  client.close();
}

TEST_CASE( "Testing SSL Client (with connect string wss://)" ) {
  std::cout << "Test 2" << std::endl;
  WebsocketsClient client;
  REQUIRE( client.connect("wss://echo.websocket.org/") == true );
  
  REQUIRE( client.send("Hello Server") == true);
  auto message = client.readBlocking();
  REQUIRE( message.data() == "Hello Server" );
  REQUIRE( client.available() == true );
  client.close();
}