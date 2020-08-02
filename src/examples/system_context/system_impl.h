#ifndef SYSTEM_POSIX_H
#define SYSTEM_POSIX_H

#include <ws.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>

#include <stdexcept>
#include <thread>

namespace Posix
{

class ContextImpl final : public WS::Network::Context
{
  public:
  virtual WS::Network::TcpListenSocketInstance createListenSocket(int port);
};

class TcpSocketImpl final : public WS::Network::TcpSocket
{
  public:
  explicit TcpSocketImpl(int fd) : fd(fd)
  {
    // make sure socket is non-blocking
  }
  ~TcpSocketImpl() override { ::close(fd); }
  ssize_t read(std::array<std::uint8_t, WS::MAX_SIZE>& buf) override;
  ssize_t write(void const* buf, std::size_t buflen) override;

  private:
  TcpSocketImpl(TcpSocketImpl const&) = delete;
  TcpSocketImpl& operator=(TcpSocketImpl const&) = delete;
  TcpSocketImpl(TcpSocketImpl&&) = delete;
  TcpSocketImpl& operator=(TcpSocketImpl&&) = delete;

  int const fd;
};

class PosixTcpListenSocket final : public WS::Network::TcpListenSocket
{
  public:
  explicit PosixTcpListenSocket(int fd) : fd{fd} {}
  ~PosixTcpListenSocket() override { ::close(fd); }

  WS::Network::TcpSocketInstance accept() override;

  private:
  PosixTcpListenSocket(PosixTcpListenSocket const&) = delete;
  PosixTcpListenSocket& operator=(PosixTcpListenSocket const&) = delete;
  PosixTcpListenSocket(PosixTcpListenSocket&&) = delete;
  PosixTcpListenSocket& operator=(PosixTcpListenSocket&&) = delete;

  int const fd;
};

} // namespace Posix

#endif /* SYSTEM_POSIX_H */
