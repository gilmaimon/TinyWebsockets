#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/internals/data_frame.hpp>

namespace websockets {
    // The class the user will interact with as a message
    // This message can be partial (so practically this is a Frame and not a message)
    enum class MessageType {
        Empty,
        Text, Binary,
        Ping, Pong, Close
    };

    MessageType messageTypeFromOpcode(uint8_t opcode);

    enum class MessageRole {
        Complete, First, Continuation, Last 
    };

    struct WebsocketsMessage {
        WebsocketsMessage(MessageType msgType, WSInterfaceString msgData, MessageRole role = MessageRole::Complete) : _type(msgType), _data(msgData), _role(role) {}
        WebsocketsMessage() : WebsocketsMessage(MessageType::Empty, "", MessageRole::Complete) {}

        static WebsocketsMessage CreateFromFrame(internals::WebsocketsFrame frame, MessageType overrideType = MessageType::Empty) {
            auto type = overrideType;
            if(type == MessageType::Empty) {
                type = messageTypeFromOpcode(frame.opcode);
            }

            // deduce role
            MessageRole role;
            if(frame.isNormalUnfragmentedMessage()) {
                role = MessageRole::Complete;
            } else if(frame.isBeginningOfFragmentsStream()) {
                role = MessageRole::First;
            } else if(frame.isContinuesFragment()) {
                role = MessageRole::Continuation;
            } else if(frame.isEndOfFragmentsStream()) {
                role = MessageRole::Last;
            }

            return WebsocketsMessage(type, frame.payload, role);
        }
        
        // for validation
        bool isEmpty() const { return this->_type == MessageType::Empty; }

        // Type Helper Functions
        MessageType type() const { return this->_type; }

        bool isText() const { return this->_type == MessageType::Text; }
        bool isBinary() const { return this->_type == MessageType::Binary; }
        
        bool isPing() const { return this->_type == MessageType::Ping; }
        bool isPong() const { return this->_type == MessageType::Pong; }
        
        bool isClose() const { return this->_type == MessageType::Close; }

        
        // Role Helper Function
        MessageRole role() const { return this->_role; }

        bool isComplete() const { return this->_role == MessageRole::Complete; }
        bool isPartial() const { return this->_role != MessageRole::Complete; }
        bool isFirst() const { return this->_role == MessageRole::First; }
        bool isContinuation() const { return this->_role == MessageRole::Continuation; }
        bool isLast() const { return this->_role == MessageRole::Last; }


        WSInterfaceString data() const { return this->_data; }

        class StreamBuilder {
        public:
            StreamBuilder(bool dummyMode = false) : _dummyMode(dummyMode), _empty(true) {}

            void first(const internals::WebsocketsFrame& frame) {
                if(this->_empty == false) {
                    badFragment();
                    return;
                }

                this->_empty = false;
                if(frame.isBeginningOfFragmentsStream()) {
                    this->_isComplete = false;
                    this->_didErrored = false;

                    if(this->_dummyMode == false) {
                        this->_content = frame.payload;
                    }

                    this->_type = messageTypeFromOpcode(frame.opcode);
                    if(this->_type == MessageType::Empty) {
                        badFragment();
                    }
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
                    if(this->_dummyMode == false) {
                        this->_content += frame.payload;
                    }
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
                    if(this->_dummyMode == false) {
                        this->_content += frame.payload;
                    }
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
            
            MessageType type() {
                return this->_type;
            }

            WebsocketsMessage build() {
                return WebsocketsMessage(
                    this->_type, 
                    std::move(this->_content),
                    MessageRole::Complete
                );
            }

        private:
            bool _dummyMode;
            bool _empty;
            bool _isComplete;
            WSString _content;
            MessageType _type;
            bool _didErrored;
        };

    private:
        const MessageType _type;
        const WSInterfaceString _data;
        const MessageRole _role;
    };
}