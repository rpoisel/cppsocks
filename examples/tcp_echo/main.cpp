#include <socks_tcp.h>
#include <system_impl.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>
#include <set>

using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::ServerHandler;

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
  void canSend(Socks::Network::Tcp::Connection* connection) { (void)connection; }

  private:
  EchoHandler(EchoHandler const&) = delete;
  EchoHandler& operator=(EchoHandler const&) = delete;
  EchoHandler(EchoHandler&&) = delete;
  EchoHandler& operator=(EchoHandler&&) = delete;
};

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  EchoHandler echoHandler;
  Socks::Network::Tcp::Server server;

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, echoHandler, Socks::Network::Tcp::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
