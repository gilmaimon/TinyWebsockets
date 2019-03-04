[![Build Status](https://travis-ci.org/gilmaimon/TinyWebsockets.svg?branch=master)](https://travis-ci.org/gilmaimon/TinyWebsockets)

# Tiny Websockets

A minimal websockets implementation in C++. Aimed to provide a simple and slim interface for basic Websockets work. Currently contains a `WebsocketsClient` and a `WebsocketsServer`.

Key Features:

* Supports [RFC-6455](https://tools.ietf.org/html/rfc6455) features: **Pings**, **Pongs**, **Data Messages** ect.
* A Modern **Callbacks-Based interface** and a **Blocking (sync) interface**.
* **Cross Platform**, support: **Windows**, **Linux**, and **Arduino** (ESP8266 and ESP32, see [ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets)) 
* Websockets **Client** and **Server** APIs.
* Fully supports **Fragmented Messages** and streaming

## Getting Started

### Installation

In order to get started and building the first TinyWebsockets demo on your machine, you should:

1. Clone the repo

```sh
git clone https://github.com/gilmaimon/TinyWebsockets.git
```

2. Run cmake

```sh
mkdir build
cd build
cmake ..
make demo_echo_client
```

3. Run the demo:

```sh
./demo_echo_client
```

### Linking to Your Own Project

Using **cmake**, add the library to the executable by adding:

```cmake
  target_link_libraries (my_executable tiny_websockets_lib)
```

## Examples

This section shows some basic examples for server and client code using the library. More demos can be found in the root library directory (`.cpp` files with the prefix `demo_`).

### Client

Example of basic `WebsocketsClient` usage:

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

### Server

Example of basic `WebsocketsServer` usage:

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

There are many issues and missing features that can use a hand so Pull requests are welcomed.

For major changes, please open an issue first to discuss what you would like to change.

### How to Contribute

The best and easiest way to help is to clone the library, test it for yourself and share any difficulties you have. You can share your opinion and experience on the interface, complexity or any issues and suggestions you have for the library.

### Missing Features

* Support for SSL/WSS - currently the library has no support for secure connections.

## License

This project is Open Source and is licensed under the [GNU General Public License v3.0](LICENSE).