#ifndef SYSTEM_POSIX_HPP
#define SYSTEM_POSIX_HPP

#include <socks_tcp.hpp>

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
  explicit SocketImpl(int fd);
  ~SocketImpl() override {}
  ssize_t read(std::array<Socks::Byte, Socks::Network::Tcp::MAX_SIZE>& buf) override;
  ssize_t write(Socks::Byte const* buf, std::size_t buflen) override;
  virtual bool isReadable() const override;
  virtual bool isWriteable() const override;
  void close() override;

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

#if SOCKS_HEADER_ONLY == 1
#include <system_posix.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SYSTEM_POSIX_HPP */
