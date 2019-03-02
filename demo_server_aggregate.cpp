/*
  TODO: this demos are not actually demos but more a manual tiny_websockets
  they should be removed from public master branch
*/

#define SERVER_PORT 8086

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <iostream>
#include <string>

using namespace websockets;

int main() {
  WebsocketsServer server;
  server.listen(SERVER_PORT);

  // while the server is alive
  while(server.available()) {
    // accept a client
    WebsocketsClient client = server.accept();
    client.setFragmentsPolicy(FragmentsPolicy_Aggregate);
    
    client.onEvent([](auto& c, WebsocketsEvent event, auto data){
      std::cout << std::endl << "Server Got an event with data: " << data << std::endl;
    });
    
    client.onMessage([](auto& c, auto msg){
      std::cout << std::endl << "Server Got: " << msg.data() << std::endl;
    });

    while(client.available()) {
      client.poll();
      std::cout << ".";
      system("sleep 0.5");
    }
    
    // close the connection
    client.close();
  }
}