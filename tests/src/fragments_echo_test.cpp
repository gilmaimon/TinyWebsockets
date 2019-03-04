#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <tiny_websockets/client.hpp>

using namespace websockets;

TEST_CASE( "Testing sending fragmented messages - Receiving the messages with Aggregate policy" ) {
  WebsocketsClient client;
  client.setFragmentsPolicy(FragmentsPolicy_Aggregate);
  REQUIRE( client.connect("ws://echo.websocket.org") == true );
  volatile bool done = false;

  client.onMessage([&done](WebsocketsClient&, auto message){
      REQUIRE( message.isContinuation() == false );
      std::cout << (message.isContinuation()? "(continuation) " : "") << "Message Got: " << message.data() << std::endl;
      REQUIRE( message.data() == "Msg: Hello World." );
      done = true;
  });

  // Stream "Msg: Hello World."
  client.stream("Msg: ");

  // send a ping in the middle of the stream, should not change anything
  client.ping("PingData");

  client.send("Hello World");
  client.end(".");

  REQUIRE( client.available() == true);
  while(!done) client.poll();
}

TEST_CASE( "Testing sending fragmented messages - Receiving the messages with Notify policy" ) {
  WebsocketsClient client;
  client.setFragmentsPolicy(FragmentsPolicy_Notify);
  REQUIRE( client.connect("ws://echo.websocket.org") == true );
  volatile int recvdCount = 0;

  client.onMessage([&recvdCount](WebsocketsClient&, auto message){
      std::cout << (message.isContinuation()? "(continuation) " : "") << "Message Got: " << message.data() << std::endl;
      REQUIRE( message.isContinuation() == true );
      if(recvdCount == 0) {
        REQUIRE( message.data() == "Msg: " );
      } else if(recvdCount == 1) {
        REQUIRE( message.data() == "Hello World" );
      } else if(recvdCount == 2) {
        REQUIRE( message.data() == "." );
      } else if(recvdCount == 3) {
        REQUIRE( "THIS STATE SHOULD NOT OCCUR" == nullptr);
        recvdCount = 3;
      }
      ++recvdCount;
  });

  // Stream "Msg: Hello World."
  client.stream("Msg: ");

  // send a ping in the middle of the stream, should not change anything
  client.ping("PingData");
  
  client.send("Hello World");
  client.end(".");

  REQUIRE( client.available() == true);
  while(recvdCount < 3) client.poll();
}

TEST_CASE( "Testing sending Receiving fragmented messages with readBlocking with Aggregate Policy" ) {
  WebsocketsClient client;
  client.setFragmentsPolicy(FragmentsPolicy_Aggregate);
  REQUIRE( client.connect("ws://echo.websocket.org") == true );

  // Stream "Msg: Hello World."
  client.stream("Msg: ");
  client.send("Hello World");
  client.end(".");

  auto message = client.readBlocking();
  
  REQUIRE( message.isContinuation() == false );
  REQUIRE( message.isText() == true );
  REQUIRE( message.data() == "Msg: Hello World." );

  REQUIRE( client.available() == true);
}
