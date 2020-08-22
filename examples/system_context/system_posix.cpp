#include <system_impl.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <functional>

using namespace Socks;

constexpr static int const BACKLOG = 5;

namespace Posix
{
namespace Network
{
namespace Tcp
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

Socks::Network::Tcp::ListenSocketInstance ContextImpl::createListenSocket(int port)
{
  auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd == RETVAL_ERROR)
  {
    throw std::runtime_error(strerror(errno));
  }
  setSocketReuseAddress(fd);
  setSocketNonBlocking(fd);

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
  return Socks::Network::Tcp::ListenSocketInstance(new ListenSocket(fd));
}

ssize_t SocketImpl::read(std::array<std::uint8_t, Socks::Network::Tcp::MAX_SIZE>& buf)
{
  ssize_t numRecv;
  numRecv = ::recv(fd, buf.data(), buf.size(), 0);
  if (numRecv == -1)
  {
    if (errno == EINTR || errno == EWOULDBLOCK)
    {
      return Socks::Network::Tcp::Socket::NUM_CONTINUE;
    }
    throw std::runtime_error(strerror(errno));
  }

  return numRecv;
}

ssize_t SocketImpl::write(void const* buf, std::size_t buflen) { return ::send(fd, buf, buflen, 0); }
bool internalPoll(int fd, short int flags, int timeout)
{
  struct pollfd pollFd = {.fd = fd, .events = flags, .revents = 0};
  auto r = poll(&pollFd, 1, timeout);
  if (r == -1)
  {
    if (errno != EINTR && errno != EAGAIN)
    {
      throw std::runtime_error(strerror(errno));
    }
  }

  return r != 0 && (pollFd.revents & flags);
}
bool SocketImpl::isReadable() const { return internalPoll(fd, POLLIN, 10 /* ms */); }
bool SocketImpl::isWriteable() const { return internalPoll(fd, POLLOUT, 0); }
void SocketImpl::close() { ::close(fd); }

Socks::Network::Tcp::SocketInstance ListenSocket::accept()
{
  if (!internalPoll(fd, POLLIN, 10 /* ms */))
  {
    return Socks::Network::Tcp::SocketInstance(nullptr);
  }
  auto client_fd = ::accept(fd, nullptr, nullptr);
  if (client_fd == RETVAL_ERROR)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
    {
      return Socks::Network::Tcp::SocketInstance(nullptr);
    }
    throw std::runtime_error(strerror(errno));
  }
  return Socks::Network::Tcp::SocketInstance(new SocketImpl(client_fd));
}


} // namespace Tcp
} // namespace Network
} // namespace Posix

namespace Socks
{

namespace Network
{
namespace Tcp
{
ssize_t const Socket::NUM_EOF = 0;
ssize_t const Socket::NUM_CONTINUE = -1;
} // namespace Tcp
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
} // namespace Socks
