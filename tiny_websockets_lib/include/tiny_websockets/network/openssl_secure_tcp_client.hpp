#pragma once

#include <tiny_websockets/internals/ws_common.hpp>
#include <tiny_websockets/network/tcp_client.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace websockets { namespace network { namespace internals {
  SSL_CTX *InitSSL_CTX(void);
}}}

namespace websockets { namespace network {
  template <class TcpClientImpl>
  class OpenSSLSecureTcpClient : public TcpClientImpl {
  public:
      OpenSSLSecureTcpClient(int s = -1) : TcpClientImpl(s) {}

      bool connect(WSString host, int port) override {
        ctx = internals::InitSSL_CTX();
        ssl = SSL_new(ctx);
        if (ssl == nullptr) {
          //fprintf(stdout, "SSL_new() failed\n");
          return false;
        }
        bool didConnect = TcpClientImpl::connect(host, port);
        if(didConnect == false) return false;

        SSL_set_fd(ssl, this->getSocket());

        const int status = SSL_connect(ssl);
        if (status != 1) {
          //fprintf(stdout, "SSL_connect failed with SSL_get_error code %d\n", status);
          return false;
        }

        return true;
      }
      void send(uint8_t* data, uint32_t len) override {
        SSL_write(ssl, data, len);
      }
      WSString readLine() override {
        uint8_t byte = '0';
        WSString line;
        read(&byte, 1);
        while (this->available()) {
          line += static_cast<char>(byte);
          if (byte == '\n') break;
          read(&byte, 1);
        }
        if(!this->available()) close();
        return line;
      }
      void send(WSString data) override {
        this->send(reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str())), data.size());
      }
      void read(uint8_t* buffer, uint32_t len) override {
        SSL_read(ssl, buffer, len);
      }
      
      void close() override {
        if(ssl != nullptr) {
          SSL_free(ssl);
          ssl = nullptr;
        }
        if(ctx != nullptr) {
          SSL_CTX_free(ctx);
          ctx = nullptr;
        }
        TcpClientImpl::close();
      }

      virtual ~OpenSSLSecureTcpClient() {
        close();
      }
  private:
      //SOCKET _socket;
      SSL_CTX *ctx;
      SSL *ssl;
  };
}} // websockets::network
