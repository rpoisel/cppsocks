#ifndef SOCKS_TCP_OPTIONS_HPP
#define SOCKS_TCP_OPTIONS_HPP

#include <cstddef>

namespace Socks
{

namespace Network
{
namespace Tcp
{
struct ServerOptions
{
  ServerOptions(int serverPort = 5555, std::size_t maxClients = 2) : serverPort(serverPort), maxClients(maxClients) {}
  int const serverPort;
  std::size_t const maxClients;
};
} // namespace Tcp
} // namespace Network
} // namespace Socks

#endif /* SOCKS_TCP_OPTIONS_H */
