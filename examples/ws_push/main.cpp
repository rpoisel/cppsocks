#include <handler.hpp>
#include <manager.hpp>
#include <pusher.hpp>

#include <socks_http.hpp>
#include <socks_http_handler.hpp>
#include <socks_ws_handler.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <exception>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>

using Posix::Network::Tcp::ContextImpl;
using Socks::Network::Http::HttpHandlerNullFactory;
using Socks::Network::Http::Server;
using Socks::Network::Http::ServerOptions;
using Socks::Network::Http::WsConnection;
using Socks::Network::Http::WsHandler;
using Socks::Network::Http::WsHandlerFactory;
using Socks::Network::Http::WsHandlerFactoryDefault;
using Socks::Network::Http::WsHandlerInstance;
using Socks::Network::Tcp::Connection;

int main()
{
  int retVal = EXIT_SUCCESS;
  ContextImpl systemContextImpl;
  ConnMgr connMgr;
  HttpHandlerNullFactory httpHandlerFactory;
  WsPushHandlerFactory wsHandlerFactory{connMgr};
  std::thread pusherThr{pusherRunner, std::ref(connMgr)};

  Socks::System::initQuitCondition();

  try
  {
    Server::serve(systemContextImpl, httpHandlerFactory, wsHandlerFactory, ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    retVal = EXIT_FAILURE;
  }
  pusherThr.join();
  return retVal;
}
