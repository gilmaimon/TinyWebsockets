#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/internals/data_frame.hpp>

namespace websockets {
    // The class the user will interact with as a message
    // This message can be partial (so practically this is a Frame and not a message)
    struct WebsocketsMessage {
        WebsocketsMessage(internals::ContentType msgType, WSInterfaceString msgData) : _type(msgType), _data(msgData) {}
        WebsocketsMessage() : WebsocketsMessage(internals::ContentType::Continuation, "") {}

        static WebsocketsMessage CreateBinary(WSInterfaceString msgData) {
            return WebsocketsMessage(internals::ContentType::Binary, msgData);
        }
        
        static WebsocketsMessage CreateText(WSInterfaceString msgData) {
            return WebsocketsMessage(internals::ContentType::Text, msgData);
        }

        static WebsocketsMessage CreateFromFrame(internals::WebsocketsFrame frame) {
            // TODO FIX frame opcodes from partial frames
            return WebsocketsMessage(
                static_cast<internals::ContentType>(frame.opcode),
                internals::fromInternalString(frame.payload)
            );
        }
        
        bool isText() const { return this->_type == internals::ContentType::Text; }
        bool isBinary() const { return this->_type == internals::ContentType::Binary; }
        
        bool isPing() const { return this->_type == internals::ContentType::Ping; }
        bool isPong() const { return this->_type == internals::ContentType::Pong; }
        
        bool isClose() const { return this->_type == internals::ContentType::Close; }

        bool isContinuation() const { return this->_type == internals::ContentType::Continuation; }

        WSInterfaceString data() const { return this->_data; }

        class StreamBuilder {
        public:
            StreamBuilder() : _empty(true) {}

            void first(const internals::WebsocketsFrame& frame) {
                if(this->_empty == false) {
                    badFragment();
                    return;
                }

                this->_empty = false;
                if(frame.isBeginningOfFragmentsStream()) {
                    this->_isComplete = false;
                    this->_didErrored = false;
                    this->_content = frame.payload;
                    this->_type = static_cast<internals::ContentType>(frame.opcode);
                } else {
                    this->_didErrored = true;
                }
            }

            void append(const internals::WebsocketsFrame& frame) {
                if(isErrored()) return;
                if(isEmpty() || isComplete()) {
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
                if(isEmpty() || isComplete()) {
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

            bool isOk() {
                return !this->_didErrored;
            }

            bool isComplete() {
                return this->_isComplete;
            }

            bool isEmpty() {
                return this->_empty;
            }

            WebsocketsMessage build() {
                return WebsocketsMessage(this->_type, std::move(this->_content));
            }

        private:
            bool _empty;
            bool _isComplete;
            WSString _content;
            internals::ContentType _type;
            bool _didErrored;
        };

    private:
        const internals::ContentType _type;
        const WSInterfaceString _data;
    };
}