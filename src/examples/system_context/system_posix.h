#ifndef SYSTEM_POSIX_H
#define SYSTEM_POSIX_H

#include <ws.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <string.h>

#include <stdexcept>
#include <thread>

class PosixContext final : public WS::SystemContext
{
  public:
  virtual WS::TcpListenSocketInstance createListenSocket(int port);
};

class PosixTcpSocket final : public WS::TcpSocket
{
  public:
  explicit PosixTcpSocket(int fd) : fd(fd)
  {
    // make sure socket is non-blocking
  }
  ~PosixTcpSocket() override { ::close(fd); }
  ssize_t read(std::array<std::uint8_t, WS::MAX_SIZE>& buf) override { return ::recv(fd, buf.data(), buf.size(), 0); }
  ssize_t write(void const* buf, std::size_t buflen) override { return ::send(fd, buf, buflen, 0); }

  private:
  PosixTcpSocket(PosixTcpSocket const&) = delete;
  PosixTcpSocket& operator=(PosixTcpSocket const&) = delete;
  PosixTcpSocket(PosixTcpSocket&&) = delete;
  PosixTcpSocket& operator=(PosixTcpSocket&&) = delete;

  int const fd;
};

class PosixTcpListenSocket final : public WS::TcpListenSocket
{
  public:
  explicit PosixTcpListenSocket(int fd) : fd{fd} {}
  ~PosixTcpListenSocket() override { ::close(fd); }

  bool waitForConnection() override
  {
    std::this_thread::sleep_for(std::chrono::duration<uint32_t, std::milli>(100));
    return true;
  }

  WS::TcpSocketInstance accept() override
  {
    auto client_fd = ::accept(fd, nullptr, nullptr);
    if (client_fd == -1)
    {
      close(fd);
      throw std::runtime_error(strerror(errno));
    }

    return WS::TcpSocketInstance(new PosixTcpSocket(client_fd));
  }

  private:
  PosixTcpListenSocket(PosixTcpListenSocket const&) = delete;
  PosixTcpListenSocket& operator=(PosixTcpListenSocket const&) = delete;
  PosixTcpListenSocket(PosixTcpListenSocket&&) = delete;
  PosixTcpListenSocket& operator=(PosixTcpListenSocket&&) = delete;

  int const fd;
};

#endif /* SYSTEM_POSIX_H */
