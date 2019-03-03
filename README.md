[![Build Status](https://travis-ci.org/gilmaimon/TinyWebsockets.svg?branch=master)](https://travis-ci.org/gilmaimon/TinyWebsockets)

# Tiny Websockets

A minimal websockets implementation in C++. Aimed to provide a simple and slim interface for basic Websockets work. Currently contains a `WebsocketsClient` and a `WebsocketsServer`.

Some Features:
* Supports [RFC-6455](https://tools.ietf.org/html/rfc6455) features: **Pings**, **Pongs**, **Data** (binary and text) Messages and more.
* Modern interface, provides both a **Callbacks-Based interface** and a **Blocking (sync) interface**.
* **Cross Platform**, supports: Windows, Linux, and Arduino (ESP8266 and ESP32, see [ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets)) 
* **Websockets Server** *(beta)*
* Fully supports **fragmented messages** and streaming

## Getting Started
In order to get started and building the first TinyWebsockets demo on your machine, you should:
1. Clone the repo
```git
git clone https://github.com/gilmaimon/TinyWebsockets.git
```
2. Run cmake
```
mkdir build
cd build
cmake ..
make demo_echo_client
```
3. Run the demo:
```
./demo_echo_client
```

## Client
Example of basic WebsocketsClient usage:
```c++
#include <tiny_websockets/client.hpp>
#include <iostream>

using namespace websockets;

int main() {
    // connect to host
    WebsocketsClient client;
    client.connect("http://localhost:8080");
    
    // handle messages
    client.onMessage([](WebsocketsMessage message) {
        std::cout << "Got: " << message.data() << std::endl;
    });
    
    // handle events
    client.onEvent([](WebsocketsEvent event, std::string data) {
        // Handle "Pings", "Pongs" and other events 
    });
    
    // send a message
    client.send("Hi Server!");
    
    while(client.available()) {
        // wait for server messages and events
        client.poll();
    }
}
```

# Server
Example of basic WebsocketsServer usage:
```c++
#include <tiny_websockets/server.hpp>
#include <iostream>

using namespace websockets;

int main() {
  WebsocketsServer server;
  server.listen(8080);
  
  // while possible
  while(server.available()) {
    // accept another client
    WebsocketsClient client = server.accept();
    
    // wait for a message for the client and send an echo response
    auto message = client.readBlocking();
    client.send("Echo: " + message.data());
    
    // close the connection
    client.close();
  }
}

```

## Contributing
There are many issues and missing features that can use a hand so Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Missing Features:
* Support for SSL/WSS
