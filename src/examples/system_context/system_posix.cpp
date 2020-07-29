#include <system_posix.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace WS;

constexpr static int const BACKLOG = 5;

TcpListenSocketInstance PosixContext::createListenSocket(int port)
{
  auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
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
  return TcpListenSocketInstance(new PosixTcpListenSocket(fd));
}
