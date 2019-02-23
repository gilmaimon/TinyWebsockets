#include <tiny_websockets/internals/ws_common.hpp>

namespace websockets { namespace internals {
    WSString fromInterfaceString(WSInterfaceString& str) {
        return std::move(str);
    }
    WSString fromInterfaceString(WSInterfaceString&& str) {
        return std::move(str);
    }

    WSInterfaceString fromInternalString(WSString& str) {
        return std::move(str);
    }
    WSInterfaceString fromInternalString(WSString&& str) {
        return std::move(str);
    }
}}