#ifndef WS_H
#define WS_H

#include <system_context.h>
#include <ws_error.h>

#include <cstddef>
#include <memory>
#include <queue>

namespace WS
{

struct ServerOptions
{
  ServerOptions(int serverPort = 5555, std::size_t maxClients = 5) : serverPort(serverPort), maxClients(maxClients) {}
  int const serverPort;
  std::size_t const maxClients;
};

class Server;

class Connection
{
  public:
  Connection(Server* server) : _server(server) {}

  Server* server() const { return _server; }
  void send(void const* buf, std::size_t len)
  {
    (void)buf;
    (void)len;
  }

  private:
  Connection(Connection&) = delete;
  Connection& operator=(Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(Connection&&) = delete;

  Server* const _server;
};

class ServerHandler
{
  public:
  virtual ~ServerHandler() {}
  virtual void onConnect(Connection* connection) = 0;
  virtual void onDisconnect(Connection* connection) = 0;
  virtual void onReceive(Connection* connection, void const* buf, std::size_t len) = 0;
};

class Server
{
  public:
  Server() = default;
  virtual ~Server() = default;
  void serve(SystemContext& systemContext, ServerHandler& handler, ServerOptions const& options);

  private:
  Server(Server&) = delete;
  Server& operator=(Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};

} // namespace WS

#endif /* WS_H */
