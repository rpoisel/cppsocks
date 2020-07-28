#ifndef WS_OPTIONS_H
#define WS_OPTIONS_H

#include <cstddef>

namespace WS
{
struct ServerOptions
{
  ServerOptions(int serverPort = 5555, std::size_t maxClients = 2) : serverPort(serverPort), maxClients(maxClients) {}
  int const serverPort;
  std::size_t const maxClients;
};
} // namespace WS

#endif // WS_OPTIONS_H