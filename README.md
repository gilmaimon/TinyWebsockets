[![Build Status](https://travis-ci.org/gilmaimon/TinyWebsockets.svg?branch=master)](https://travis-ci.org/gilmaimon/TinyWebsockets)

# Tiny Websockets

A minimal websockets implementation in C++. Aimed to provide a simple and slim interface for basic Websockets work.

## Basic Usage
Client usage for Windows:
```c++
using namespace websockets;

WebsocketsClient client;
client.connect("localhost", "/", 8080);

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