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
static constexpr int const RETVAL_ERROR = -1;

static void setSocketFlags(int fd, std::function<int(int)> manipulate)
{
  int flags;
  while ((flags = ::fcntl(fd, F_GETFL, 0)) == RETVAL_ERROR)
  {
    if (errno == EINTR)
    {
      continue;
    }
    std::runtime_error(strerror(errno));
  }
  flags = manipulate(flags);
  while (::fcntl(fd, F_SETFL, flags) == RETVAL_ERROR)
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

static void setSocketReuseAddress(int fd)
{
  int val = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, (socklen_t)sizeof(val)) == RETVAL_ERROR)
  {
    throw std::runtime_error(strerror(errno));
  }
}

Network::TcpListenSocketInstance ContextImpl::createListenSocket(int port)
{
  auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd == RETVAL_ERROR)
  {
    throw std::runtime_error(strerror(errno));
  }
  setSocketReuseAddress(fd);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  auto rc = ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  if (rc == RETVAL_ERROR)
  {
    close(fd);
    throw std::runtime_error(strerror(errno));
  }

  rc = ::listen(fd, BACKLOG);
  if (rc == RETVAL_ERROR)
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
      return Network::TcpSocket::NUM_CONTINUE;
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
  if (r == RETVAL_ERROR)
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
  if (client_fd == RETVAL_ERROR)
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

namespace Network
{
ssize_t const TcpSocket::NUM_EOF = 0;
ssize_t const TcpSocket::NUM_CONTINUE = -1;
} // namespace Network
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
