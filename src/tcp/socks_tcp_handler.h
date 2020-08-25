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
  Connection(Network::Tcp::SocketInstance socket, Server* server) : socket{socket}, _server{server}, closed{false} {}
  Connection(Connection&& other) : socket{std::move(other.socket)}, _server{other._server}, closed{other.closed}
  {
    other.socket = SocketInstance();
    other._server = nullptr;
  }

  Server* server() const { return _server; }
  std::size_t send(Byte const* buf, std::size_t len)
  {
    std::size_t numWritten = 0;
    while (!closed && numWritten < len)
    {
      numWritten += socket->write(buf + numWritten, len - numWritten);
    }
    return numWritten;
  }
  std::size_t send(char const* buf, std::size_t len) { return send(reinterpret_cast<Byte const*>(buf), len); }
  void close() { closed = true; }
  bool isClosed() const { return closed; }

  private:
  Connection() = delete;
  Connection(Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  Network::Tcp::SocketInstance socket;
  Server* _server;
  bool closed;
};

class ConnectionHolder
{
  public:
  ConnectionHolder(Network::Tcp::SocketInstance socket, Server* server) : connection_(socket, server) {}
  virtual ~ConnectionHolder() = default;

  Connection* connection() { return &connection_; }

  private:
  Connection connection_;
};

class ServerHandler : public ConnectionHolder
{
  public:
  ServerHandler(Network::Tcp::SocketInstance socket, Server* server) : ConnectionHolder{socket, server} {}
  virtual ~ServerHandler() {}
  virtual void onConnect() = 0;
  virtual void onDisconnect() = 0;
  virtual void onReceive(Byte const* buf, std::size_t len) = 0;
};

using ServerHandlerInstance = std::unique_ptr<ServerHandler>;

class ServerHandlerFactory
{
  public:
  virtual ~ServerHandlerFactory() = default;

  virtual ServerHandlerInstance createServerHandler(Network::Tcp::SocketInstance socket, Server* server) = 0;
};

} // namespace Tcp
} // namespace Network

} // namespace Socks

#endif // SOCKS_TCP_HANDLER_H
