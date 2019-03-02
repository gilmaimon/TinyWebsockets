#pragma once

#include <tiny_websockets/internals/ws_common.hpp>

namespace websockets { namespace internals {
  struct WebsocketsFrame {
    uint8_t fin : 1;
    uint8_t opcode : 4;
    uint8_t mask : 1;
    uint8_t mask_buf[4];
    uint64_t payload_length;
    WSString payload;

    bool isBeginningOfFragmentsStream() const {
      return (fin == 0) && (opcode != 0);
    }

    bool isContinuesFragment() const {
      return (fin == 0) && (opcode == 0);
    }

    bool isEndOfFragmentsStream() const {
      return (fin == 1) && (opcode == 0);
    }

    bool isNormalUnfragmentedMessage() const {
      return (fin == 1) && (opcode != 0);
    }
  };

  struct Header {
    uint8_t opcode : 4;
    uint8_t flags : 3;
    uint8_t fin : 1;
    uint8_t payload : 7;
    uint8_t mask : 1;
  };

  struct HeaderWithExtended : Header {
    uint16_t extendedPayload;
  };
}} // websockets::internals