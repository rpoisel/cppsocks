#include <socks_tcp.h>
#include <system_impl.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>

using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::ServerHandler;

class PushHandler final : public ServerHandler
{
  public:
  PushHandler() = default;
  ~PushHandler() = default;

  void onConnect(Connection* connection) override
  {
    spdlog::info("Connection established.");
    std::unique_lock<std::mutex> lk(mtx);
    connections.insert(connection);
  }
  void onDisconnect(Connection* connection) override
  {
    {
      std::unique_lock<std::mutex> lk(mtx);
      connections.erase(connection);
    }
    spdlog::info("Connection closed.");
  }
  void onReceive(Connection* connection, void const* buf, std::size_t len) override { connection->send(buf, len); }
  void push(void const* buf, std::size_t len)
  {
    std::unique_lock<std::mutex> lk(mtx);
    std::for_each(connections.begin(), connections.end(), [&](Connection* connection) { connection->send(buf, len); });
  }

  private:
  PushHandler(PushHandler const&) = delete;
  PushHandler& operator=(PushHandler const&) = delete;
  PushHandler(PushHandler&&) = delete;
  PushHandler& operator=(PushHandler&&) = delete;

  std::set<Connection*> connections;
  std::mutex mtx;
};

static void pusherRunner(PushHandler& pushHandler)
{
  std::stringstream msg;
  for (std::size_t cnt = 0; !Socks::System::quitCondition(); cnt++)
  {
    msg.str("");
    msg.clear();
    msg << cnt << std::endl;
    pushHandler.push(msg.str().c_str(), msg.str().length() + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

int main()
{
  int retVal = EXIT_SUCCESS;
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  PushHandler pushHandler;
  Socks::Network::Tcp::Server server;
  std::thread pusherThr{pusherRunner, std::ref(pushHandler)};

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, pushHandler, Socks::Network::Tcp::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    retVal = EXIT_FAILURE;
  }
  pusherThr.join();
  return retVal;
}
