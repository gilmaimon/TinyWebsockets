#pragma once
#include <string>
#include <functional>

typedef std::string String;

class WebsocketsMessage;
typedef std::function<void(WebsocketsMessage)> MessageCallback;