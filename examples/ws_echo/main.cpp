#include <socks_http.hpp>
#include <socks_http_handler.hpp>
#include <socks_ws_handler.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <cstring>

using Posix::Network::Tcp::ContextImpl;
using Socks::Network::Http::HttpHandlerNullFactory;
using Socks::Network::Http::Server;
using Socks::Network::Http::ServerOptions;
using Socks::Network::Http::WsHandler;
using Socks::Network::Http::WsHandlerFactory;
using Socks::Network::Http::WsHandlerFactoryDefault;
using Socks::Network::Http::WsHandlerInstance;
using Socks::Network::Tcp::Connection;

class WsEchoHandler final : public WsHandler
{
  public:
  WsEchoHandler(Connection* tcpConnection) : WsHandler(tcpConnection) {}
  void onConnect() override { spdlog::info("WebSocket connect."); }
  void onText(char const* buf, std::size_t len) override { connection()->send(buf, len); }
  void onData(Socks::Byte const* buf, std::size_t len) override { connection()->send(buf, len); }
  void onDisconnect() override { spdlog::info("WebSocket disconnect."); }
};

int main()
{
  ContextImpl systemContextImpl;
  HttpHandlerNullFactory httpHandlerFactory;
  class WsHandlerFactoryDefault<WsEchoHandler> wsHandlerFactory;

  Socks::System::initQuitCondition();

  try
  {
    Server::serve(systemContextImpl, httpHandlerFactory, wsHandlerFactory, ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
