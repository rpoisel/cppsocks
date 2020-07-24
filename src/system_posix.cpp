#include <system_posix.h>

using namespace WS;

TcpListenSocketInstance PosixContext::createListenSocket(int port)
{
  // socket
  // bind
  ::listen(port, 10);
  return TcpListenSocketInstance(new PosixTcpListenSocket(10 /* TODO */));
}
