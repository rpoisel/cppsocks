#include <system_posix.h>
#include <ws.h>

#include <exception>
#include <iostream>
#include <set>

class EchoHandler : public WS::ServerHandler
{
  public:
  EchoHandler() = default;
  ~EchoHandler() = default;

  void onConnect(WS::Connection* connection) override { connections.insert(connection); }
  void onDisconnect(WS::Connection* connection) override { connections.erase(connection); }
  void onReceive(WS::Connection* connection, void const* buf, std::size_t len) override { connection->send(buf, len); }

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
