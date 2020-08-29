#include <socks_tcp.hpp>
#include <socks_tcp_types.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>
#include <set>

using Socks::Byte;
using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::Server;
using Socks::Network::Tcp::ServerHandler;
using Socks::Network::Tcp::ServerHandlerFactory;
using Socks::Network::Tcp::ServerHandlerInstance;

class EchoHandler final : public ServerHandler
{
  public:
  EchoHandler(Socks::Network::Tcp::SocketInstance socket, Server* server) : ServerHandler(socket, server) {}

  void onConnect() override { spdlog::info("Connection established."); }
  void onDisconnect() override { spdlog::info("Connection closed."); }
  void onReceive(Byte const* buf, std::size_t len) override
  {
    spdlog::info("Got message with length {} bytes.", len);
    connection()->send(buf, len);
    spdlog::info("Sent message with length {} bytes.", len);
  }

  private:
  EchoHandler(EchoHandler const&) = delete;
  EchoHandler& operator=(EchoHandler const&) = delete;
  EchoHandler(EchoHandler&&) = delete;
  EchoHandler& operator=(EchoHandler&&) = delete;
};

class EchoHandlerFactory final : public ServerHandlerFactory
{
  public:
  ServerHandlerInstance createServerHandler(Socks::Network::Tcp::SocketInstance socket, Server* server) override
  {
    return ServerHandlerInstance(new EchoHandler(socket, server));
  }
};

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  EchoHandlerFactory echoHandlerFactory;
  Socks::Network::Tcp::Server server;

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, echoHandlerFactory, Socks::Network::Tcp::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
