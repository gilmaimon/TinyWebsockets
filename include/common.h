#pragma once
#include <string>
#include <functional>

namespace websockets {
    typedef std::string String;

    class WebsocketsMessage;
    typedef std::function<void(WebsocketsMessage)> MessageCallback;
}