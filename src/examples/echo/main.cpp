#include <system_impl.h>
#include <ws.h>

#include <exception>
#include <iostream>
#include <set>

using WS::Connection;
using WS::ServerHandler;

class EchoHandler final : public ServerHandler
{
  public:
  EchoHandler() = default;
  ~EchoHandler() = default;

  void onConnect(Connection* connection) override
  {
    std::cout << "Connection established." << std::endl;
    connections.insert(connection);
  }
  void onDisconnect(Connection* connection) override
  {
    connections.erase(connection);
    std::cout << "Connection closed." << std::endl;
  }
  void onReceive(Connection* connection, void const* buf, std::size_t len) override { connection->send(buf, len); }

  private:
  EchoHandler(EchoHandler const&) = delete;
  EchoHandler& operator=(EchoHandler const&) = delete;
  EchoHandler(EchoHandler&&) = delete;
  EchoHandler& operator=(EchoHandler&&) = delete;

  std::set<WS::Connection*> connections;
};

int main()
{
  PosixContext posixContext;
  EchoHandler echoHandler;
  WS::Server server;

  try
  {
    server.serve(posixContext, echoHandler, WS::ServerOptions());
  }
  catch (std::exception& exc)
  {
    std::cerr << "An error occured: " << exc.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
