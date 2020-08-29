#include <socks_http.hpp>
#include <socks_http_states.hpp>
#include <socks_tcp.hpp>
#include <socks_tcp_handler.hpp>
#include <socks_tcp_options.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Socks
{
namespace Network
{
namespace Http
{

using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::Context;
using Socks::Network::Tcp::ServerHandler;
using Socks::Network::Tcp::ServerHandlerFactory;
using Socks::Network::Tcp::ServerHandlerInstance;

class TcpServerHandler final : public ServerHandler, HttpStateContext
{
  public:
  explicit TcpServerHandler(Socks::Network::Tcp::SocketInstance socket, Socks::Network::Tcp::Server* server,
                            HttpHandlerFactory& httpHandlerFactory, WsHandlerFactory& wsHandlerFactory)
      : ServerHandler{socket, server}, httpHandlerFactory_(httpHandlerFactory),
        wsHandlerFactory_(wsHandlerFactory), requestInfo_{}, curState{new HttpInitState(this, requestInfo_)}, nextState{}
  {
  }

  void setNextState(HttpStateInstance nextState) override
  {
    nextState->onEnter();
    this->nextState = std::move(nextState);
  }
  RequestInfo const& requestInfo() override { return requestInfo_; }
  HttpHandlerFactory& httpHandlerFactory() override { return httpHandlerFactory_; }
  WsHandlerFactory& wsHandlerFactory() override { return wsHandlerFactory_; }
  Socks::Network::Tcp::Connection* tcpConnection() override { return connection(); }

  void onConnect() override {}
  void onDisconnect() override { curState->onDisconnect(); }
  void onReceive(Byte const* buf, std::size_t len) override
  {
    curState->onReceive(buf, len);
    if (nextState.get())
    {
      curState = std::move(nextState);
      nextState = HttpStateInstance();
    }
  }

  private:
  HttpHandlerFactory& httpHandlerFactory_;
  WsHandlerFactory& wsHandlerFactory_;
  RequestInfo requestInfo_;
  HttpStateInstance curState;
  HttpStateInstance nextState;
};

class TcpServerHandlerFactory final : public ServerHandlerFactory
{
  public:
  TcpServerHandlerFactory(HttpHandlerFactory& httpHandlerFactory, WsHandlerFactory& wsHandlerFactory)
      : httpHandlerFactory(httpHandlerFactory), wsHandlerFactory(wsHandlerFactory)
  {
  }

  ServerHandlerInstance createServerHandler(Socks::Network::Tcp::SocketInstance socket, Socks::Network::Tcp::Server* server)
  {
    auto tcpServerHandler = new TcpServerHandler(socket, server, httpHandlerFactory, wsHandlerFactory);
    return ServerHandlerInstance(tcpServerHandler);
  }

  private:
  HttpHandlerFactory& httpHandlerFactory;
  WsHandlerFactory& wsHandlerFactory;
};

SOCKS_INLINE void Server::serve(Context& context, HttpHandlerFactory& httpHandlerFactory, WsHandlerFactory& wsHandlerFactory,
                                ServerOptions const& options)
{
  TcpServerHandlerFactory tcpHandlerFactory(httpHandlerFactory, wsHandlerFactory);
  Socks::Network::Tcp::ServerOptions tcpOptions(options.port, options.maxClients);
  Socks::Network::Tcp::Server server;

  server.serve(context, tcpHandlerFactory, tcpOptions);
}
} // namespace Http
} // namespace Network
} // namespace Socks
