#include <system_impl.h>
#include <ws.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

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
    (void)connection;
    spdlog::info("Connection established.");
  }
  void onDisconnect(Connection* connection) override
  {
    (void)connection;
    spdlog::info("Connection closed.");
  }
  void onReceive(Connection* connection, void const* buf, std::size_t len) override
  {
    spdlog::info("Got message with length {} bytes.", len);
    connection->send(buf, len);
    spdlog::info("Sent message with length {} bytes.", len);
  }

  private:
  EchoHandler(EchoHandler const&) = delete;
  EchoHandler& operator=(EchoHandler const&) = delete;
  EchoHandler(EchoHandler&&) = delete;
  EchoHandler& operator=(EchoHandler&&) = delete;
};

int main()
{
  SystemContextImpl systemContextImpl;
  EchoHandler echoHandler;
  WS::Server server;

  try
  {
    server.serve(systemContextImpl, echoHandler, WS::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
