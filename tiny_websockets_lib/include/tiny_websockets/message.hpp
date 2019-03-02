#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/internals/data_frame.hpp>

namespace websockets {
    enum MessageType {
        // Default value for empty messages
        Empty = 0,

        // Data opcdoes
        Text = 0x1,
        Binary = 0x2,

        // Control opcodes
        Close = 0x8,
        Ping = 0x9,
        Pong = 0xA
    };

    // The class the user will interact with as a message
    // This message can be partial (so practically this is a Frame and not a message)
    struct WebsocketsMessage {
        WebsocketsMessage(MessageType msgType, WSInterfaceString msgData) : _type(msgType), _data(msgData) {}
        WebsocketsMessage() : WebsocketsMessage(MessageType::Empty, "") {}

        static WebsocketsMessage CreateBinary(WSInterfaceString msgData) {
            return WebsocketsMessage(MessageType::Binary, msgData);
        }
        
        static WebsocketsMessage CreateText(WSInterfaceString msgData) {
            return WebsocketsMessage(MessageType::Text, msgData);
        }

        static WebsocketsMessage CreateFromFrame(internals::WebsocketsFrame frame) {
            // TODO FIX frame opcodes from partial frames
            return WebsocketsMessage(
                static_cast<MessageType>(frame.opcode),
                internals::fromInternalString(frame.payload)
            );
        }
        
        bool isText() const { return this->_type == MessageType::Text; }
        bool isBinary() const { return this->_type == MessageType::Binary; }

        MessageType type() const { return this->_type; }
        WSInterfaceString data() const { return this->_data; }

        class StreamBuilder {
        public:
            StreamBuilder(const internals::WebsocketsFrame& frame) {
                if(frame.isBeginningOfFragmentsStream()) {
                    this->_isComplete = false;
                    this->_didErrored = false;
                    this->_content = frame.payload;
                    this->_type = static_cast<MessageType>(frame.opcode);
                } else {
                    this->_didErrored = true;
                }
            }

            void append(const internals::WebsocketsFrame& frame) {
                if(isErrored()) return;
                if(isComplete()) {
                    badFragment();
                    return;
                }

                if(frame.isContinuesFragment()) {
                    this->_content += frame.payload;
                } else {
                    badFragment();
                }
            }

            void end(const internals::WebsocketsFrame& frame) {
                if(isErrored()) return;
                if(isComplete()) {
                    badFragment();
                    return;
                }

                if(frame.isEndOfFragmentsStream()) {
                    this->_content += frame.payload;
                    this->_isComplete = true;
                } else {
                    badFragment();
                }
            }

            void badFragment() {
                this->_didErrored = true;
                this->_isComplete = false;
            }

            bool isErrored() {
                return this->_didErrored;
            }

            bool isComplete() {
                return this->_isComplete;
            }

            WebsocketsMessage build() {
                return WebsocketsMessage(this->_type, std::move(this->_content));
            }

        private:
            bool _isComplete;
            WSString _content;
            MessageType _type;
            bool _didErrored;
        };

    private:
        MessageType _type;
        WSInterfaceString _data;
    };
}