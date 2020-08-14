#include <socks_http.h>

#include <socks_tcp.h>
#include <socks_tcp_options.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Socks
{
namespace Network
{
namespace Http
{

class TcpServerHandler final : public Socks::Network::Tcp::ServerHandler
{
  public:
  explicit TcpServerHandler(Socks::Network::Http::HttpHandler& httpHandler) : httpHandler{httpHandler} {}
  ~TcpServerHandler() = default;

  void onConnect(Socks::Network::Tcp::Connection* connection) { (void)connection; }
  void onDisconnect(Socks::Network::Tcp::Connection* connection) { (void)connection; }
  void onReceive(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len)
  {
    try
    {
      RequestInfo requestInfo(static_cast<char const*>(buf), len);
      Socks::Network::Http::HttpConnection httpConnection(connection);
      if (requestInfo.requestType() == RequestType::GET)
      {
        httpHandler.do_GET(&httpConnection, &requestInfo);
      }
    }
    catch (const std::exception& exc)
    {
      spdlog::error("{}", exc.what());
    }

    connection->close();
  }

  private:
  Socks::Network::Http::HttpHandler& httpHandler;
};

void Server::serve(Socks::Network::Tcp::Context& context, HttpHandler& httpHandler, WSHandler& wsHandler,
                   ServerOptions const& options)
{
  (void)wsHandler;

  TcpServerHandler tcpHandler(httpHandler);
  Socks::Network::Tcp::ServerOptions tcpOptions(options.port, options.maxClients);
  Socks::Network::Tcp::Server server;

  server.serve(context, tcpHandler, tcpOptions);
}
} // namespace Http
} // namespace Network
} // namespace Socks