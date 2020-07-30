#include <system_impl.h>
#include <ws.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <thread>

using WS::Connection;
using WS::ServerHandler;

class PushHandler final : public ServerHandler
{
  public:
  PushHandler() = default;
  ~PushHandler() = default;

  void onConnect(Connection* connection) override
  {
    std::cout << "Connection established." << std::endl;
    std::unique_lock<std::mutex> lk(mtx);
    connections.insert(connection);
  }
  void onDisconnect(Connection* connection) override
  {
    {
      std::unique_lock<std::mutex> lk(mtx);
      connections.erase(connection);
    }
    std::cout << "Connection closed." << std::endl;
  }
  void onReceive(Connection* connection, void const* buf, std::size_t len) override { connection->send(buf, len); }
  void push(void const* buf, std::size_t len)
  {
    std::unique_lock<std::mutex> lk(mtx);
    for (auto& connection : connections)
    {
      connection->send(buf, len);
    }
  }

  private:
  PushHandler(PushHandler const&) = delete;
  PushHandler& operator=(PushHandler const&) = delete;
  PushHandler(PushHandler&&) = delete;
  PushHandler& operator=(PushHandler&&) = delete;

  std::set<WS::Connection*> connections;
  std::mutex mtx;
};

static void pusherRunner(PushHandler& pushHandler)
{
  std::stringstream msg;
  for (std::size_t cnt = 0;; cnt++)
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
  SystemContextImpl systemContextImpl;
  PushHandler pushHandler;
  WS::Server server;
  std::thread pusherThr{pusherRunner, std::ref(pushHandler)};

  try
  {
    server.serve(systemContextImpl, pushHandler, WS::ServerOptions());
  }
  catch (std::exception& exc)
  {
    std::cerr << "An error occured: " << exc.what() << std::endl;
    retVal = EXIT_FAILURE;
  }
  pusherThr.join();
  return retVal;
}
