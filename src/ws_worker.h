#ifndef WS_WORKER_H
#define WS_WORKER_H

#include <system_context.h>
#include <ws_handler.h>

#include <thread>

namespace WS
{

class ClientWorker
{
  public:
  ClientWorker(TcpSocketInstance socket, Server* server) : socket(std::move(socket)), conn{server}, thr{} {}
  ClientWorker(ClientWorker&& other)
      : socket{std::move(other.socket)}, conn{std::move(other.conn)}, thr{std::move(other.thr)}
  {
  }
  virtual ~ClientWorker() {}

  void start() { thr = std::thread{&ClientWorker::run, this}; }
  void run()
  {
    if (httpHandshake())
    {
      return;
    }
    wsHandshake();
  }
  void finish() { thr.join(); }
  bool isActive() const { return !thr.joinable(); }

  private:
  ClientWorker(ClientWorker&) = delete;
  ClientWorker& operator=(ClientWorker&) = delete;
  ClientWorker& operator=(ClientWorker&&) = delete;

  bool httpHandshake() { return false; }
  void wsHandshake() {}

  TcpSocketInstance socket;
  Connection conn;
  std::thread thr;
};

} // namespace WS

#endif /* WS_WORKER _H */