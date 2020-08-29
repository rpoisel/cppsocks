#ifndef SOCKS_HTTP_HANDLER_HPP
#define SOCKS_HTTP_HANDLER_HPP

#include <socks_tcp_handler.hpp>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
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
  GET_WS,
};

using HeadersMap = std::map<std::string const, std::string const>;

class RequestInfo
{
  public:
  RequestInfo() : _requestType{UNDEFINED}, _path{}, _headers{} {}
  virtual ~RequestInfo() = default;

  void parse(char const* request, std::size_t len);
  RequestType requestType() const { return _requestType; }
  char const* path() const { return _path; }
  HeadersMap const& headers() const { return _headers; }

  private:
  constexpr static std::size_t const MAX_PATH_LEN = 1024;

  RequestType _requestType;
  char _path[MAX_PATH_LEN];
  HeadersMap _headers;
};

class HttpConnection
{
  public:
  using ResponseCode = std::uint16_t;

  explicit HttpConnection(Socks::Network::Tcp::Connection* tcpConnection);

  void http_200(std::streampos contentLength);
  inline void http_403() { http_response_code("403 Forbidden"); }
  inline void http_404() { http_response_code("404 Not Found"); }
  inline void http_500() { http_response_code("500 Internal Server Error"); }

  void write(char const* buf, std::size_t len);

  private:
  void http_response_code(char const* responseCode);
  void end_headers();

  Socks::Network::Tcp::Connection* tcpConnection;
};

class HttpHandler
{
  public:
  virtual ~HttpHandler() = default;
  virtual void do_GET(HttpConnection* connection, RequestInfo const& requestInfo) = 0;
};

using HttpHandlerInstance = std::shared_ptr<HttpHandler>;

class HttpHandlerFactory
{
  public:
  virtual ~HttpHandlerFactory() = default;

  virtual HttpHandlerInstance createHttpHandler() = 0;
};

class HttpFileHandler final : public HttpHandler
{
  public:
  explicit HttpFileHandler(std::string const& basePath) : basePath(basePath) {}
  void do_GET(HttpConnection* connection, RequestInfo const& requestInfo) override;

  private:
  std::string const basePath;
};

class HttpFileHandlerFactory final : public HttpHandlerFactory
{
  public:
  explicit HttpFileHandlerFactory(std::string const& basePath) : basePath(basePath) {}

  HttpHandlerInstance createHttpHandler() override { return HttpHandlerInstance(new HttpFileHandler(basePath)); }

  private:
  std::string const& basePath;
};

class HttpHandlerNull final : public HttpHandler
{
  void do_GET(HttpConnection* connection, RequestInfo const& requestInfo) override
  {
    (void)connection;
    (void)requestInfo;
  }
};

class HttpHandlerNullFactory final : public HttpHandlerFactory
{
  public:
  HttpHandlerInstance createHttpHandler() override { return HttpHandlerInstance(new HttpHandlerNull()); }
};
} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_HTTP_HANDLER_HPP */
