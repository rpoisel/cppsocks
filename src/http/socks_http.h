#ifndef SOCKS_HTTP_H
#define SOCKS_HTTP_H

#include <socks_http_handler.h>
#include <socks_tcp_handler.h>

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
  Server() = default;
  virtual ~Server() = default;
  void serve(Socks::Network::Tcp::Context& context, HttpHandler& httpHandler, WSHandler& wsHandler,
             ServerOptions const& options = ServerOptions());
  inline void serve(Socks::Network::Tcp::Context& context, WSHandler& wsHandler, std::string const& basePath,
                    ServerOptions const& options = ServerOptions())
  {
    HttpFileHandler fileHandler(basePath);
    serve(context, fileHandler, wsHandler, options);
  }
  inline void serve(Socks::Network::Tcp::Context& context, HttpHandler& httpHandler,
                    ServerOptions const& options = ServerOptions())
  {
    WSHandlerNull wsHandler;
    serve(context, httpHandler, wsHandler, options);
  }
  inline void serve(Socks::Network::Tcp::Context& context, std::string const& basePath,
                    ServerOptions const& options = ServerOptions())
  {
    HttpFileHandler fileHandler(basePath);
    WSHandlerNull wsHandler;
    serve(context, fileHandler, wsHandler, options);
  }

  private:
  Server(Server&) = delete;
  Server& operator=(Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};
} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_HTTP_H */