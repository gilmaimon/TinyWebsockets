#include <tiny_websockets/internals/ws_common.hpp>

namespace websockets { namespace internals {
    WSString fromInterfaceString(WSInterfaceString& str) {
        return str;
    }
    WSString fromInterfaceString(WSInterfaceString&& str) {
        return str;
    }

    WSInterfaceString fromInternalString(WSString& str) {
        return str;
    }
    WSInterfaceString fromInternalString(WSString&& str) {
        return str;
    }
}}