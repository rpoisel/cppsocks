#ifndef WS_HANDLER_H
#define WS_HANDLER_H

#include <ws_fwd.h>

#include <cstddef>

namespace WS
{
class Connection
{
  public:
  Connection(Server* server) : _server{server} {}
  Connection(Connection&& other) : _server{other._server} { other._server = nullptr; }

  Server* server() const { return _server; }
  void send(void const* buf, std::size_t len)
  {
    (void)buf;
    (void)len;
  }

  private:
  Connection(Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

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