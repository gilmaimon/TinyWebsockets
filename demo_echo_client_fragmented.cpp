/*
  TODO: this demos are not actually demos but more a manual tiny_websockets
  they should be removed from public master branch
*/

#include <tiny_websockets/client.hpp>
#include <tiny_websockets/server.hpp>
#include <iostream>

using namespace websockets;

int main() {
  WebsocketsClient clientAggregate;
  clientAggregate.connect("ws://localhost:8086/");

  WebsocketsClient clientFragmented;
  clientFragmented.connect("ws://localhost:8087/");

  clientFragmented.onMessage([&](WebsocketsClient&, WebsocketsMessage message){
    std::cout << "Got" << (message.isContinuation()? " (continSuation)": "") << " Data: " << message.data() << std::endl;
  });
  clientAggregate.onMessage([&](WebsocketsClient&, WebsocketsMessage message){
    std::cout << "Got" << (message.isContinuation()? " (continSuation)": "") << " Data: " << message.data() << std::endl;
  });

  WSString line;
  while(clientAggregate.available() && clientFragmented.available()) {
    std::cout << "Enter input: ";
    std::getline(std::cin, line);

    if(line != "") {
      if(line == "exit") {
        clientAggregate.close();
        clientFragmented.close();
      }
      else {
        clientAggregate.poll();
        clientFragmented.poll();
        
        // Stream the string ("User Said: " + line + ". Thats it!") to the server
        // This is done in 3 seperate messages 
        clientAggregate.stream("So, "); // starts streaming
        system("sleep 2");
        // sends continuous (fragmented) messages
        clientAggregate.send("User Said: `");
        system("sleep 2");
        clientAggregate.send(line);
        system("sleep 2");
        clientAggregate.send("`. Thats it!");
        system("sleep 2");

        // ends the stream
        clientAggregate.end(".");


        system("sleep 2");
        clientFragmented.stream("So, "); // starts streaming

        // sends continuous (fragmented) messages
        clientFragmented.send("User Said: `");
        system("sleep 2");
        clientFragmented.send(line);
        system("sleep 2");
        clientFragmented.send("`. Thats it!");
        system("sleep 2");

        // ends the stream
        clientFragmented.end(".");
      }
    }
    clientAggregate.poll();
    clientFragmented.poll();
  }
  std::cout << "Exited Gracefully" << std::endl;
}