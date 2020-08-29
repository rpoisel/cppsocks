#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <manager.hpp>

#include <socks_http.hpp>
#include <socks_ws_handler.hpp>

using ConnMgr = class ElemMgr<Socks::Network::Http::WsConnection>;

class WsPushHandler final : public Socks::Network::Http::WsHandler
{
  public:
  WsPushHandler(Socks::Network::Tcp::Connection* tcpConnection, ConnMgr& connMgr)
      : WsHandler(tcpConnection), connMgr(connMgr)
  {
  }

  void onConnect() override;
  void onDisconnect() override;
  void onData(Socks::Byte const* buf, std::size_t len) override;

  private:
  WsPushHandler(WsPushHandler const&) = delete;
  WsPushHandler& operator=(WsPushHandler const&) = delete;
  WsPushHandler(WsPushHandler&&) = delete;
  WsPushHandler& operator=(WsPushHandler&&) = delete;

  ConnMgr& connMgr;
};

class WsPushHandlerFactory final : public Socks::Network::Http::WsHandlerFactory
{
  public:
  WsPushHandlerFactory(ConnMgr& connMgr) : connMgr(connMgr) {}

  Socks::Network::Http::WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection) override
  {
    return Socks::Network::Http::WsHandlerInstance(new WsPushHandler(tcpConnection, connMgr));
  }

  private:
  ConnMgr& connMgr;
};

#endif /* HANDLER_HPP */
