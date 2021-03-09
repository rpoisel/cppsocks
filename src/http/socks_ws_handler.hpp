#ifndef SOCKS_WS_HANDLER_HPP
#define SOCKS_WS_HANDLER_HPP

#include <socks_tcp_handler.hpp>

#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>


namespace Socks
{
namespace Network
{
namespace Http
{

class WsConnection final
{
  public:
  explicit WsConnection(Socks::Network::Tcp::Connection* tcpConnection) : tcpConnection{tcpConnection} {}
  std::size_t send(Byte const* buf, std::size_t len);
  std::size_t send(char const* buf);
  void close();

  private:
  Socks::Network::Tcp::Connection* tcpConnection;
};

class WsHandler
{
  public:
  WsHandler(Socks::Network::Tcp::Connection* tcpConnection) : connection_{tcpConnection} {}
  virtual ~WsHandler() = default;

  virtual void onConnect() = 0;
  virtual void onData(Byte const* buf, std::size_t len) = 0;
  virtual void onText(char const* buf) { onData(reinterpret_cast<Byte const*>(buf), std::strlen(buf)); }
  virtual void onDisconnect() = 0;
  virtual std::string subprotocol() { return std::string(); }

  virtual WsConnection* connection() final { return &connection_; }

  private:
  WsConnection connection_;
};

using WsHandlerInstance = std::shared_ptr<WsHandler>;

class WsHandlerFactory
{
  public:
  virtual ~WsHandlerFactory() = default;

  virtual WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection) = 0;
};

template <class T>
class WsHandlerFactoryDefault : public WsHandlerFactory
{
  WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection)
  {
    return WsHandlerInstance(new T(tcpConnection));
  }
};


template <class T, class L, class C>
class WsHandlerFactoryMultiClient : public WsHandlerFactory
{
  public:
  WsHandlerFactoryMultiClient() = default;

  WsHandlerFactoryMultiClient(L& lock, C& content) : lock(lock), content(content){};

  WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection)
  {
    T* newConnection = new T(tcpConnection);

    lock.lock();
    content.newClient(newConnection);
    lock.unlock();

    return WsHandlerInstance(new T(tcpConnection));
  }

  private:
  L& lock;
  C& content;
};


class WsHandlerNull final : public WsHandler
{
  public:
  explicit WsHandlerNull(Socks::Network::Tcp::Connection* tcpConnection) : WsHandler{tcpConnection} {}
  void onConnect() override {}
  void onData(Byte const* buf, std::size_t len) override
  {
    (void)buf;
    (void)len;
  }
  void onDisconnect() override {}
};

class WsHandlerNullFactory final : public WsHandlerFactory
{
  public:
  WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection) override
  {
    static auto wsHandlerNull = WsHandlerInstance(new WsHandlerNull(tcpConnection));
    return wsHandlerNull;
  }
};

} // namespace Http
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_ws_handler.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_WS_HANDLER_HPP */
