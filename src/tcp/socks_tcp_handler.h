#ifndef SOCKS_TCP_HANDLER_H
#define SOCKS_TCP_HANDLER_H

#include <socks_system_context.h>
#include <socks_tcp_fwd.h>

#include <cstddef>
#include <memory>

namespace Socks
{

namespace Network
{
namespace Tcp
{
class Connection
{
  public:
  Connection(Network::Tcp::Socket* socket, Server* server) : socket{socket}, _server{server}, closed{false} {}
  Connection(Connection&& other) : socket{other.socket}, _server{other._server}, closed{other.closed}
  {
    other.socket = nullptr;
    other._server = nullptr;
  }

  Server* server() const { return _server; }
  void send(void const* buf, std::size_t len) { socket->write(buf, len); }
  void close() { closed = true; }
  bool isClosed() const { return closed; }

  private:
  Connection(Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  Network::Tcp::Socket* socket;
  Server* _server;
  bool closed;
};

class ServerHandler
{
  public:
  virtual ~ServerHandler() {}
  virtual void onConnect(Connection* connection) = 0;
  virtual void onDisconnect(Connection* connection) = 0;
  virtual void onReceive(Connection* connection, void const* buf, std::size_t len) = 0;
  virtual void canSend(Connection* connection) = 0;
};

using ServerHandlerInstance = std::unique_ptr<ServerHandler>;

class ServerHandlerFactory
{
  public:
  ServerHandlerFactory() = default;
  virtual ~ServerHandlerFactory() = default;

  virtual ServerHandlerInstance createServerHandler() = 0;
};

} // namespace Tcp
} // namespace Network

} // namespace Socks

#endif // SOCKS_TCP_HANDLER_H
