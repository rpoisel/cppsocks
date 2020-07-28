#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <ws_types.h>

#include <unistd.h>

#include <array>
#include <cstddef>
#include <memory>

namespace WS
{

class TcpSocket
{
  public:
  static constexpr std::size_t const MAX_SIZE = 1024;
  using MsgBuf = std::array<Byte, MAX_SIZE>;

  virtual ~TcpSocket() {}
  virtual ssize_t read(MsgBuf& buf) = 0;
  virtual ssize_t write(void const* buf, std::size_t buflen) = 0;
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