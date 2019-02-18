#include "catch.hpp"
#include "wscrypto/crypto.h"

TEST_CASE( "Testing Base64 Encode" ) {
    REQUIRE (websockets::crypto::base64Encode("0123456789ABCDEF") == "MDEyMzQ1Njc4OUFCQ0RFRg==");
    REQUIRE (websockets::crypto::base64Encode("AAAFFFVVS1SSAG5H") == "QUFBRkZGVlZTMVNTQUc1SA==");
}

TEST_CASE( "Testing Base64 Decode" ) {
    REQUIRE (websockets::crypto::base64Decode("MDEyMzQ1Njc4OUFCQ0RFRg==") == "0123456789ABCDEF");
    REQUIRE (websockets::crypto::base64Decode("QUFBRkZGVlZTMVNTQUc1SA==") == "AAAFFFVVS1SSAG5H");
}

TEST_CASE( "Testing Websockets Handshake" ) {
    REQUIRE (websockets::crypto::websocketsHandshakeEncodeKey("dGhlIHNhbXBsZSBub25jZQ==") == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
    REQUIRE (websockets::crypto::websocketsHandshakeEncodeKey("x3JJHMbDL1EzLkh9GBhXDw==") == "HSmrc0sMlYUkAGmm5OPpG2HaGWk=");
}

TEST_CASE( "Testing Random Bytes" ) {
    #ifdef _WS_CONFIG_NO_TRUE_RANDOMNESS
        REQUIRE (websockets::crypto::randomBytes(16) == "0123456789abcdef");
    #else
        REQUIRE (websockets::crypto::randomBytes(16) != websockets::crypto::randomBytes(16));
    #endif
}