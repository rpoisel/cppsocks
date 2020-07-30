#ifndef WS_HANDLER_H
#define WS_HANDLER_H

#include <system_context.h>
#include <ws_fwd.h>

#include <cstddef>

namespace WS
{
class Connection
{
  public:
  Connection(TcpSocket* socket, Server* server) : socket{socket}, _server{server} {}
  Connection(Connection&& other) : socket{other.socket}, _server{other._server} { other._server = nullptr; }

  Server* server() const { return _server; }
  void send(void const* buf, std::size_t len) { socket->write(buf, len); }

  private:
  Connection(Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  TcpSocket* socket;
  Server* _server;
};

class ServerHandler
{
  public:
  virtual ~ServerHandler() {}
  virtual void onConnect(Connection* connection) = 0;
  virtual void onDisconnect(Connection* connection) = 0;
  virtual void onReceive(Connection* connection, void const* buf, std::size_t len) = 0;
};

} // namespace WS

#endif // WS_HANDLER_H