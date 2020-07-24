#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <cstddef>
#include <memory>

namespace WS
{

class TcpSocket
{
  public:
  virtual ~TcpSocket() {}
  virtual std::size_t read() = 0;
  virtual void write(void const* buf, std::size_t len) = 0;
};

using TcpSocketInstance = std::unique_ptr<TcpSocket>;

class TcpListenSocket
{
  public:
  virtual ~TcpListenSocket() {}
  virtual bool waitForConnection() = 0;
  virtual TcpSocketInstance accept() = 0;
};

using TcpListenSocketInstance = std::unique_ptr<TcpListenSocket>;

class SystemContext
{
  public:
  virtual ~SystemContext() {}
  virtual TcpListenSocketInstance createListenSocket(int port) = 0;
};

} // namespace WS

#endif /* SYSTEM_CONTEXT_H */