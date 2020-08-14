#ifndef SOCKS_HTTP_HANDLER_H
#define SOCKS_HTTP_HANDLER_H

#include <socks_tcp_handler.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace Socks
{
namespace Network
{
namespace Http
{

enum RequestType : std::uint8_t
{
  UNDEFINED = 0,
  GET,
};

class RequestInfo
{
  public:
  explicit RequestInfo(char const* request, std::size_t len);
  virtual ~RequestInfo() {}

  RequestType requestType() const { return _requestType; }
  char const* path() const { return _path; }

  private:
  constexpr static std::size_t const MAX_PATH_LEN = 1024;

  RequestInfo(RequestInfo&) = delete;
  RequestInfo(RequestInfo&& other) = delete;
  RequestInfo& operator=(RequestInfo&) = delete;
  RequestInfo& operator=(RequestInfo&&) = delete;

  RequestType _requestType;
  char _path[MAX_PATH_LEN];
};

class HttpConnection
{
  public:
  using ResponseCode = std::uint16_t;

  explicit HttpConnection(Socks::Network::Tcp::Connection* tcpConnection);
  virtual ~HttpConnection() {}

  void http_200(std::streampos contentLength);
  inline void http_403() { http_response_code("403 Forbidden"); }
  inline void http_404() { http_response_code("404 Not Found"); }
  inline void http_500() { http_response_code("500 Internal Server Error"); }

  void write(char const* buf, std::size_t len);

  private:
  HttpConnection(HttpConnection&) = delete;
  HttpConnection(HttpConnection&& other) = delete;
  HttpConnection& operator=(HttpConnection&) = delete;
  HttpConnection& operator=(HttpConnection&&) = delete;

  void http_response_code(char const* responseCode);
  void end_headers();

  Socks::Network::Tcp::Connection* tcpConnection;
};

class HttpHandler
{
  public:
  HttpHandler() = default;
  virtual ~HttpHandler() {}
  virtual void do_GET(HttpConnection* connection, RequestInfo const* requestInfo) = 0;
};

class HttpFileHandler final : public HttpHandler
{
  public:
  explicit HttpFileHandler(std::string const& basePath) : basePath(basePath) {}
  ~HttpFileHandler() = default;
  void do_GET(HttpConnection* connection, RequestInfo const* requestInfo);

  private:
  std::string const basePath;
};

class WSHandler
{
  public:
  WSHandler() = default;
  virtual ~WSHandler() {}
  virtual void onConnect() = 0;
  virtual void onData() = 0;
  virtual void onDisconnect() = 0;
};

class WSHandlerNull final : public WSHandler
{
  public:
  WSHandlerNull() = default;
  ~WSHandlerNull() = default;

  void onConnect() {}
  void onData() {}
  void onDisconnect() {}
};

} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_HTTP_HANDLER_H */