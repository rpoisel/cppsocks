#ifndef SYSTEM_POSIX_H
#define SYSTEM_POSIX_H

#include <socks_tcp.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>

#include <stdexcept>
#include <thread>

namespace Posix
{

namespace Network
{
namespace Tcp
{

class ContextImpl final : public Socks::Network::Tcp::Context
{
  public:
  virtual Socks::Network::Tcp::ListenSocketInstance createListenSocket(int port);
};

class SocketImpl final : public Socks::Network::Tcp::Socket
{
  public:
  explicit SocketImpl(int fd) : fd(fd)
  {
    // make sure socket is non-blocking
  }
  ~SocketImpl() override { ::close(fd); }
  ssize_t read(std::array<std::uint8_t, Socks::Network::Tcp::MAX_SIZE>& buf) override;
  ssize_t write(void const* buf, std::size_t buflen) override;

  private:
  SocketImpl(SocketImpl const&) = delete;
  SocketImpl& operator=(SocketImpl const&) = delete;
  SocketImpl(SocketImpl&&) = delete;
  SocketImpl& operator=(SocketImpl&&) = delete;

  int const fd;
};

class ListenSocket final : public Socks::Network::Tcp::ListenSocket
{
  public:
  explicit ListenSocket(int fd) : fd{fd} {}
  ~ListenSocket() override { ::close(fd); }

  Socks::Network::Tcp::SocketInstance accept() override;

  private:
  ListenSocket(ListenSocket const&) = delete;
  ListenSocket& operator=(ListenSocket const&) = delete;
  ListenSocket(ListenSocket&&) = delete;
  ListenSocket& operator=(ListenSocket&&) = delete;

  int const fd;
};

} // namespace Tcp
} // namespace Network

} // namespace Posix

#endif /* SYSTEM_POSIX_H */
