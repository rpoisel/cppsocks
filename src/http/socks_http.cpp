#include <socks_http.h>

#include <socks_tcp.h>
#include <socks_tcp_options.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <functional>

namespace Socks
{
namespace Network
{
namespace Http
{

class TcpServerHandler final : public Socks::Network::Tcp::ServerHandler
{
  public:
  explicit TcpServerHandler(HttpHandlerInstance httpHandler)
      : httpHandler{std::move(httpHandler)}, recvBuf(), numRecv(0), curState{&TcpServerHandler::initHttp}
  {
  }
  ~TcpServerHandler() = default;

  void onConnect(Socks::Network::Tcp::Connection* connection) { (void)connection; }
  void onDisconnect(Socks::Network::Tcp::Connection* connection) { (void)connection; }
  void onReceive(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len)
  {
    curState(*this, connection, buf, len);
  }
  void canSend(Socks::Network::Tcp::Connection* connection) { (void)connection; }

  private:
  void initHttp(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len);
  void handleWs(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len);
  Socks::Network::Http::HttpHandlerInstance httpHandler;
  std::array<char, 4096> recvBuf;
  std::size_t numRecv;
  std::function<void(TcpServerHandler& handler, Socks::Network::Tcp::Connection* connection, void const* buf,
                     std::size_t len)>
      curState;
};

class TcpServerHandlerFactory final : public Socks::Network::Tcp::ServerHandlerFactory
{
  public:
  TcpServerHandlerFactory(HttpHandlerFactory& httpHandlerFactory) : httpHandlerFactory(httpHandlerFactory) {}
  ~TcpServerHandlerFactory() = default;

  Socks::Network::Tcp::ServerHandlerInstance createServerHandler()
  {
    auto tcpServerHandler = new TcpServerHandler(std::move(httpHandlerFactory.createHttpHandler()));
    // TODO pass wsHandler
    return Socks::Network::Tcp::ServerHandlerInstance(tcpServerHandler);
  }

  private:
  HttpHandlerFactory& httpHandlerFactory;
};

void TcpServerHandler::initHttp(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len)
{
  constexpr std::array<char, 4> TERMINATOR = {'\r', '\n', '\r', '\n'};
  try
  {
    // TODO check bounds and available buffer sizes
    std::memcpy(recvBuf.data() + numRecv, buf, len);
    numRecv += len;
    if (numRecv < recvBuf.size() &&
        std::memcmp(recvBuf.data() + numRecv - TERMINATOR.size(), TERMINATOR.data(), TERMINATOR.size()))
    {
      return;
    }
    RequestInfo requestInfo(recvBuf.data(), numRecv);
    Socks::Network::Http::HttpConnection httpConnection(connection);
    if (requestInfo.requestType() == RequestType::GET)
    {
      httpHandler->do_GET(&httpConnection, &requestInfo);
    }
    else if (requestInfo.requestType() == RequestType::GET_WS)
    {
      curState = &TcpServerHandler::handleWs;
      return;
    }
  }
  catch (const std::exception& exc)
  {
    spdlog::error("{}", exc.what());
  }

  connection->close();
}

void TcpServerHandler::handleWs(Socks::Network::Tcp::Connection* connection, void const* buf, std::size_t len)
{
  (void)connection;
  (void)buf;
  (void)len;
}

void Server::serve(Socks::Network::Tcp::Context& context, HttpHandlerFactory& httpHandlerFactory, WSHandler& wsHandler,
                   ServerOptions const& options)
{
  (void)wsHandler;

  TcpServerHandlerFactory tcpHandlerFactory(httpHandlerFactory);
  Socks::Network::Tcp::ServerOptions tcpOptions(options.port, options.maxClients);
  Socks::Network::Tcp::Server server;

  server.serve(context, tcpHandlerFactory, tcpOptions);
}
} // namespace Http
} // namespace Network
} // namespace Socks