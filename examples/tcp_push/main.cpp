#include <socks_tcp.h>
#include <system_impl.h>

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

using Socks::Byte;
using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::Server;
using Socks::Network::Tcp::ServerHandler;
using Socks::Network::Tcp::ServerHandlerFactory;
using Socks::Network::Tcp::ServerHandlerInstance;
using Socks::Network::Tcp::SocketInstance;

class ConnMgr final
{
  public:
  void addConn(Connection* connection)
  {
    std::unique_lock<std::mutex> lk(mtx);
    connections.insert(connection);
  }

  void delConn(Connection* connection)
  {
    std::unique_lock<std::mutex> lk(mtx);
    connections.erase(connection);
  }

  void push(char const* buf, std::size_t len)
  {
    std::unique_lock<std::mutex> lk(mtx);
    std::for_each(connections.begin(), connections.end(), [&](Connection* connection) { connection->send(buf, len); });
  }

  private:
  std::set<Connection*> connections;
  std::mutex mtx;
};


class PushHandler final : public ServerHandler
{
  public:
  PushHandler(SocketInstance socket, Server* server, ConnMgr& connMgr) : ServerHandler{socket, server}, connMgr(connMgr) {}

  void onConnect() override
  {
    spdlog::info("Connection established.");
    connMgr.addConn(connection());
  }
  void onDisconnect() override
  {
    connMgr.delConn(connection());
    spdlog::info("Connection closed.");
  }
  void onReceive(Byte const* buf, std::size_t len) override { connection()->send(buf, len); }

  private:
  PushHandler(PushHandler const&) = delete;
  PushHandler& operator=(PushHandler const&) = delete;
  PushHandler(PushHandler&&) = delete;
  PushHandler& operator=(PushHandler&&) = delete;

  ConnMgr& connMgr;
};

class PushHandlerFactory final : public ServerHandlerFactory
{
  public:
  PushHandlerFactory(ConnMgr& connMgr) : connMgr(connMgr) {}

  ServerHandlerInstance createServerHandler(SocketInstance socket, Server* server) override
  {
    return ServerHandlerInstance(new PushHandler(socket, server, connMgr));
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
    connMgr.push(msg.str().c_str(), msg.str().length() + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

int main()
{
  int retVal = EXIT_SUCCESS;
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  ConnMgr connMgr;
  PushHandlerFactory pushHandlerFactory{connMgr};
  Socks::Network::Tcp::Server server;
  std::thread pusherThr{pusherRunner, std::ref(connMgr)};

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, pushHandlerFactory, Socks::Network::Tcp::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    retVal = EXIT_FAILURE;
  }
  pusherThr.join();
  return retVal;
}
