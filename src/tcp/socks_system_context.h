#ifndef SOCKS_SYSTEM_CONTEXT_H
#define SOCKS_SYSTEM_CONTEXT_H

#include <socks_tcp_types.h>

#include <unistd.h>

#include <cstddef>
#include <memory>

namespace Socks
{

namespace Network
{

namespace Tcp
{
class Socket
{
  public:
  static ssize_t const NUM_EOF;
  static ssize_t const NUM_CONTINUE;

  virtual ~Socket() {}
  virtual ssize_t read(MsgBuf& buf) = 0;
  virtual ssize_t write(Byte const* buf, std::size_t buflen) = 0;
  virtual bool isReadable() const = 0;
  virtual bool isWriteable() const = 0;
  virtual void close() = 0;
};

using SocketInstance = std::shared_ptr<Socket>;

class ListenSocket
{
  public:
  virtual ~ListenSocket() {}
  virtual SocketInstance accept() = 0;
};

using ListenSocketInstance = std::unique_ptr<ListenSocket>;

class Context
{
  public:
  virtual ~Context() {}
  virtual ListenSocketInstance createListenSocket(int port) = 0;
};
} // namespace Tcp
} // namespace Network
namespace System
{
void initQuitCondition();
bool quitCondition();
} // namespace System
} // namespace Socks

#endif /* SOCKS_SYSTEM_CONTEXT_H */
