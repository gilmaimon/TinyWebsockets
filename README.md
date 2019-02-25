[![Build Status](https://travis-ci.org/gilmaimon/TinyWebsockets.svg?branch=master)](https://travis-ci.org/gilmaimon/TinyWebsockets)

# Tiny Websockets

A minimal websockets implementation in C++. Aimed to provide a simple and slim interface for basic Websockets work. Currently contains only a `WebsocketsClient`.

Some Features:
* Supports [RFC-6455](https://tools.ietf.org/html/rfc6455) features: Pings, Pongs, Data (binary and text) Messages and more.
* Has both: a callback based interface and a blocking interface.
* Cross Platform, supports: Windows, Linux, and Arduino (ESP8266 and ESP32, see [ArduinoWebsockets](https://github.com/gilmaimon/ArduinoWebsockets)) 

## Basic Usage
Client usage for Windows:
```c++
using namespace websockets;

WebsocketsClient client;
client.connect("http://localhost:8080");

client.onMessage([](WebsocketsMessage message) {
    std::cout << "Got: " << message.data() << std::endl;
});

client.onEvent([](WebsocketsEvent event, std::string data) {
    // Handle "Pings", "Pongs" and other events 
});

client.send("Hi Server!");
while(client.available()) {
    client.poll();
}
```

## Contributing
There are many issues and missing features that can use a hand so Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Missing Features:
* Websockets Server
* Support for SSL/WSS