#include <tiny_websockets/internals/ws_common.hpp>

namespace websockets { namespace internals {
    WSString fromInterfaceString(const WSInterfaceString& str) {
        return str;
    }
    WSString fromInterfaceString(const WSInterfaceString&& str) {
        return str;
    }

    WSInterfaceString fromInternalString(const WSString& str) {
        return str;
    }
    WSInterfaceString fromInternalString(const WSString&& str) {
        return str;
    }
}}