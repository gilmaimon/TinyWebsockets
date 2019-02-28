/*
  Simple Echo Server that can handle one client at a time.
  
  After running this demo, there will be a websockets server running
  on port SERVER_PORT (default 8080), that server will accept connections
  and will respond with "echo" messages for every received message (one 
  clinet at a time, no multiple connections).

  The code:
    1. Sets up a server
    2. Accepts a client
    3. While the client connected, wait for a message
    4. if that message is text, send an echo. otherwise don't do anything
    5. When the client is no longer available, close the connection and start from step 1.
*/

#define SERVER_PORT 8080

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <iostream>

using namespace websockets;

int main() {
  WebsocketsServer server;
  server.listen(SERVER_PORT);

  // while the server is alive
  while(server.available()) {
    // accept a client
    WebsocketsClient client = server.accept();
    
    while(client.available()) {
      // get a message, if it is text, return an echo
      auto message = client.readBlocking();
      if(message.isText()){
        client.send("Echo: " + message.data());
      }
    }
    
    // close the connection
    client.close();
  }
}