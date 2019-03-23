#include <tiny_websockets/network/openssl_secure_tcp_client.hpp>

#ifndef _WS_CONFIG_NO_SSL

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace websockets { namespace network { namespace internals {
  SSL_CTX *InitSSL_CTX(void) {
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    #if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_library_init();
    #else
    OPENSSL_init_ssl(0, NULL);
    #endif

    #if defined(LWS_HAVE_TLS_CLIENT_METHOD)
	  SSL_METHOD *method = const_cast<SSL_METHOD*>(TLS_client_method());
    #elif defined(LWS_HAVE_TLSV1_2_CLIENT_METHOD)
	  SSL_METHOD *method = const_cast<SSL_METHOD*>(TLSv1_2_client_method());
    #else
	  SSL_METHOD *method = const_cast<SSL_METHOD*>(SSLv23_client_method());
    #endif

    SSL_CTX *ctx = SSL_CTX_new(method);

    if (ctx == nullptr)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
  }
}}}

#endif //_WS_CONFIG_NO_SSL