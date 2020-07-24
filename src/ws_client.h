#ifndef WS_CLIENT_H
#define WS_CLIENT_H

#include <system_context.h>

#include <thread>

namespace WS
{

class ClientWorker
{
  public:
  ClientWorker(TcpSocketInstance socket) : socket(std::move(socket)), thr{&ClientWorker::run, this} {}
  virtual ~ClientWorker() {}

  void run() {}
  void finish() { thr.join(); }
  bool isActive() const { return thr.joinable(); }

  private:
  ClientWorker(ClientWorker&) = delete;
  ClientWorker& operator=(ClientWorker&) = delete;
  ClientWorker(ClientWorker&&) = delete;
  ClientWorker& operator=(ClientWorker&&) = delete;

  TcpSocketInstance socket;
  std::thread thr;
};

} // namespace WS

#endif /* WS_CLIENT _H */