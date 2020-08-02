#ifndef WS_WORKER_H
#define WS_WORKER_H

#include <system_context.h>
#include <ws_handler.h>

#include <atomic>
#include <thread>

namespace WS
{

class ClientWorker
{
  public:
  ClientWorker(Network::TcpSocketInstance socket, ServerHandler& handler, Server* server)
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
  bool httpHandshake();
  void wsLoop();

  Network::TcpSocketInstance socket;
  ServerHandler& handler;
  Connection conn;
  std::atomic_bool _isActive;
  std::thread thr;
};

} // namespace WS

#endif /* WS_WORKER _H */