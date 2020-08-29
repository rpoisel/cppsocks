#ifndef SOCKS_TCP_WORKER_HPP
#define SOCKS_TCP_WORKER_HPP

#include <socks_system_context.hpp>
#include <socks_tcp_handler.hpp>

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
  ClientWorker(SocketInstance socket, ServerHandlerInstance handler)
      : socket{socket}, handler(std::move(handler)), _isActive(false), thr{}
  {
    start();
  }
  ClientWorker(ClientWorker&& other)
      : socket{std::move(other.socket)},
        handler(std::move(other.handler)), _isActive{other._isActive ? true : false}, thr{std::move(other.thr)}
  {
  }
  virtual ~ClientWorker() = default;

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
  ServerHandlerInstance handler;
  std::atomic_bool _isActive;
  std::thread thr;
};
} // namespace Tcp
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_tcp_worker.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_TCP_WORKER _HPP */
