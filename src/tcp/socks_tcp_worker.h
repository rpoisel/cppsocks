#ifndef SOCKS_TCP_WORKER_H
#define SOCKS_TCP_WORKER_H

#include <socks_system_context.h>
#include <socks_tcp_handler.h>

#include <atomic>
#include <thread>

namespace Socks
{
namespace Network
{
namespace Tcp
{

class ClientWorker
{
  public:
  ClientWorker(SocketInstance socket, ServerHandler& handler, Server* server)
      : socket(std::move(socket)), handler(handler), conn{this->socket.get(), server}, _isActive(false), thr{}
  {
    start();
  }
  ClientWorker(ClientWorker&& other)
      : socket{std::move(other.socket)},
        handler(other.handler), conn{std::move(other.conn)}, _isActive{other._isActive ? true : false}, thr{std::move(
                                                                                                            other.thr)}
  {
  }
  virtual ~ClientWorker() {}

  void run();
  void finish() { thr.join(); }
  bool isActive() const { return _isActive; }

  private:
  ClientWorker(ClientWorker&) = delete;
  ClientWorker& operator=(ClientWorker&) = delete;
  ClientWorker& operator=(ClientWorker&&) = delete;

  void start()
  {
    _isActive = true;
    thr = std::thread{&ClientWorker::run, this};
  }
  void loop();

  SocketInstance socket;
  ServerHandler& handler;
  Connection conn;
  std::atomic_bool _isActive;
  std::thread thr;
};
} // namespace Tcp
} // namespace Network

} // namespace Socks

#endif /* SOCKS_TCP_WORKER _H */
