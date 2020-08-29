#include <socks_http.hpp>
#include <socks_http_handler.hpp>
#include <socks_ws_handler.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
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

template <typename C>
class ElemMgr final
{
  public:
  void addElem(C* elem)
  {
    std::unique_lock<std::mutex> lk(mtx);
    elements.insert(elem);
  }

  void delElem(C* elem)
  {
    std::unique_lock<std::mutex> lk(mtx);
    elements.erase(elem);
  }

  void push(Socks::Byte const* buf, std::size_t len)
  {
    std::unique_lock<std::mutex> lk(mtx);
    std::for_each(elements.begin(), elements.end(), [&](C* connection) { connection->send(buf, len); });
  }

  private:
  std::set<C*> elements;
  std::mutex mtx;
};

using ConnMgr = class ElemMgr<WsConnection>;

class WsPushHandler final : public WsHandler
{
  public:
  WsPushHandler(Socks::Network::Tcp::Connection* tcpConnection, ConnMgr& connMgr)
      : WsHandler(tcpConnection), connMgr(connMgr)
  {
  }

  void onConnect() override
  {
    spdlog::info("WebSocket connect.");
    connMgr.addElem(connection());
  }
  void onDisconnect() override
  {
    connMgr.delElem(connection());
    spdlog::info("WebSocket disconnect.");
  }
  void onData(Socks::Byte const* buf, std::size_t len) override { connection()->send(buf, len); }

  private:
  WsPushHandler(WsPushHandler const&) = delete;
  WsPushHandler& operator=(WsPushHandler const&) = delete;
  WsPushHandler(WsPushHandler&&) = delete;
  WsPushHandler& operator=(WsPushHandler&&) = delete;

  ConnMgr& connMgr;
};

class WsPushHandlerFactory final : public WsHandlerFactory
{
  public:
  WsPushHandlerFactory(ConnMgr& connMgr) : connMgr(connMgr) {}

  WsHandlerInstance createWsHandler(Socks::Network::Tcp::Connection* tcpConnection) override
  {
    return WsHandlerInstance(new WsPushHandler(tcpConnection, connMgr));
  }

  private:
  ConnMgr& connMgr;
};

static void pusherRunner(ConnMgr& connMgr)
{
  std::stringstream msg;
  for (std::size_t cnt = 0; !Socks::System::quitCondition(); cnt++)
  {
    msg.str("");
    msg.clear();
    msg << cnt << std::endl;
    connMgr.push(reinterpret_cast<Socks::Byte const*>(msg.str().c_str()), msg.str().length() + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

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
