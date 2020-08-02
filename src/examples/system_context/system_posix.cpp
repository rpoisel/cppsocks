#include <system_impl.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <functional>

using namespace WS;

constexpr static int const BACKLOG = 5;

namespace Posix
{

static void setSocketFlags(int fd, std::function<int(int)> manipulate)
{
  int flags;
  while ((flags = ::fcntl(fd, F_GETFL, 0)) == -1)
  {
    if (errno == EINTR)
    {
      continue;
    }
    std::runtime_error(strerror(errno));
  }
  flags = manipulate(flags);
  while (::fcntl(fd, F_SETFL, flags) == -1)
  {
    if (errno == EINTR)
    {
      continue;
    }
    std::runtime_error(strerror(errno));
  }
}

static void setSocketNonBlocking(int fd)
{
  setSocketFlags(fd, [](int flags) { return flags | O_NONBLOCK; });
}

Network::TcpListenSocketInstance ContextImpl::createListenSocket(int port)
{
  auto fd = ::socket(AF_INET, SOCK_STREAM, 0);

  int val = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, (socklen_t)sizeof(val)) == -1)
  {
    throw std::runtime_error(strerror(errno));
  }

  struct sockaddr_in addr;
  if (fd == -1)
  {
    throw std::runtime_error(strerror(errno));
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  auto rc = ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  if (rc == -1)
  {
    close(fd);
    throw std::runtime_error(strerror(errno));
  }

  rc = ::listen(fd, BACKLOG);
  if (rc == -1)
  {
    close(fd);
    throw std::runtime_error(strerror(errno));
  }
  setSocketNonBlocking(fd);
  return Network::TcpListenSocketInstance(new PosixTcpListenSocket(fd));
}

ssize_t Posix::TcpSocketImpl::read(std::array<std::uint8_t, WS::MAX_SIZE>& buf)
{
  ssize_t numRecv;
  numRecv = ::recv(fd, buf.data(), buf.size(), 0);
  if (numRecv == -1)
  {
    if (errno == EINTR || errno == EWOULDBLOCK)
    {
      return -1;
    }
    throw std::runtime_error(strerror(errno));
  }

  return numRecv;
}

ssize_t Posix::TcpSocketImpl::write(void const* buf, std::size_t buflen) { return ::send(fd, buf, buflen, 0); }

WS::Network::TcpSocketInstance PosixTcpListenSocket::accept()
{
  struct timeval timeout
  {
    0, 10000
  };
  fd_set read_fds;

  FD_ZERO(&read_fds);
  FD_SET(fd, &read_fds);
  auto r = ::select(fd + 1, &read_fds, nullptr, nullptr, &timeout);
  if (r == -1)
  {
    if (errno != EINTR)
    {
      throw std::runtime_error(strerror(errno));
    }
  }
  else if (r == 0)
  {
    return WS::Network::TcpSocketInstance(nullptr);
  }

  auto client_fd = ::accept(fd, nullptr, nullptr);
  if (client_fd == -1)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      return WS::Network::TcpSocketInstance(nullptr);
    }
    throw std::runtime_error(strerror(errno));
  }
  setSocketNonBlocking(client_fd);
  return WS::Network::TcpSocketInstance(new TcpSocketImpl(client_fd));
}


} // namespace Posix

namespace WS
{
namespace System
{
constexpr std::sig_atomic_t const NO_SIGNAL = 0;
static std::sig_atomic_t signalStatus = NO_SIGNAL;

static void sigHandler(int sigNo) { signalStatus = sigNo; }

void initQuitCondition()
{
  std::signal(SIGINT, sigHandler);
  std::signal(SIGTERM, sigHandler);
}
bool quitCondition() { return signalStatus != NO_SIGNAL; }
} // namespace System
} // namespace WS
