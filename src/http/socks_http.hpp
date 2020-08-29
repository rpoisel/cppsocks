#ifndef SOCKS_HTTP_HPP
#define SOCKS_HTTP_HPP

#include <socks_http_handler.hpp>
#include <socks_tcp_handler.hpp>
#include <socks_ws_handler.hpp>

#include <cstddef>

namespace Socks
{
namespace Network
{
namespace Http
{

struct ServerOptions
{
  ServerOptions(int port = 8080, std::size_t maxClients = 2) : port(port), maxClients(maxClients) {}
  int const port;
  std::size_t const maxClients;
};

class Server final
{
  public:
  static void serve(Socks::Network::Tcp::Context& context, HttpHandlerFactory& httpHandlerFactory,
                    WsHandlerFactory& wsHandlerFactory, ServerOptions const& options = ServerOptions());
  static inline void serve(Socks::Network::Tcp::Context& context, WsHandlerFactory& wsHandler, std::string const& basePath,
                           ServerOptions const& options = ServerOptions())
  {
    HttpFileHandlerFactory fileHandlerFactory(basePath);
    serve(context, fileHandlerFactory, wsHandler, options);
  }
  static inline void serve(Socks::Network::Tcp::Context& context, HttpHandlerFactory& httpHandlerFactory,
                           ServerOptions const& options = ServerOptions())
  {
    WsHandlerNullFactory wsHandlerFactory;
    serve(context, httpHandlerFactory, wsHandlerFactory, options);
  }
  static inline void serve(Socks::Network::Tcp::Context& context, std::string const& basePath,
                           ServerOptions const& options = ServerOptions())
  {
    HttpFileHandlerFactory fileHandlerFactory(basePath);
    WsHandlerNullFactory wsHandlerFactory;
    serve(context, fileHandlerFactory, wsHandlerFactory, options);
  }

  private:
  Server() = delete;
  ~Server() = delete;
  Server(Server& other) = delete;
  Server(Server&& other) = delete;
  Server& operator=(Server const& server) = delete;
  Server& operator=(Server&& server) = delete;
};
} // namespace Http
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_http.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_HTTP_HPP */
