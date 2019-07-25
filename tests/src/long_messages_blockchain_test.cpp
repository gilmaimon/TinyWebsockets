#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

#define PING_BLOCK_DATA std::string("{\"op\":\"ping_block\"}")
#define NUM_SEND_RECEIVE_ITERATIONS 10

/*
 * This odd test is here because of this issue: https://github.com/gilmaimon/ArduinoWebsockets/issues/21
 * The service tested sends very large messages, fragmented and encrypted which use to break the library
 * because of inappropriate checking. This (very specific) test is here to make sure this does not happen again. 
 */

TEST_CASE( "Testing commiunication with long fragmented encrypted messages (blockchain service)" ) {
  WebsocketsClient client;
    REQUIRE( client.connect("wss://ws.blockchain.info/inv") == true );

    REQUIRE( client.available() == true );
    
    for(int i = 0; i < NUM_SEND_RECEIVE_ITERATIONS; i++) {
      client.send(PING_BLOCK_DATA);
      while(true) {
        auto msg = client.readBlocking();
        if (msg.isText()) break;
      }
      REQUIRE( client.available() == true );
    }
}
