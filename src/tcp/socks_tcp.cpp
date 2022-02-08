#include <socks_tcp.hpp>
#include <socks_tcp_types.hpp>
#include <socks_tcp_worker.hpp>

#include <algorithm>
#include <cstring>
#include <list>

#include <string>
#include <vector>
#include <iostream>

#include <spdlog/spdlog.h>

namespace Socks
{

namespace Network
{
namespace Tcp
{

using ClientWorkers = std::list<ClientWorker>;

static void cleanup(ClientWorkers& workers);
SOCKS_INLINE void Server::serve(Context& context, ServerHandlerFactory& handlerFactory, ServerOptions const& options)
{
  ClientWorkers workers;
  auto listenSocket = context.createListenSocket(options.serverPort);
  while (!System::quitCondition())
  {
    auto clientSocket = listenSocket->accept();

    cleanup(workers);

    if (!clientSocket.get())
    {
      continue;
    }

    if (workers.size() < options.maxClients)
    {
      workers.emplace_back(clientSocket, handlerFactory.createServerHandler(clientSocket, this));
      continue;
    }
    constexpr auto const bye = "No more connections allowed.\n";
    clientSocket->write(reinterpret_cast<Socks::Byte const*>(bye), std::strlen(bye));
    std::cout << "Connection rejected because no more connections are allowed" << std::endl;
  }
  std::for_each(workers.begin(), workers.end(), [](ClientWorker& worker) { worker.finish(); });
}

static void cleanup(ClientWorkers& workers)
{
  workers.remove_if([](ClientWorker& worker) {
    if (worker.isActive())
    {
      return false;
    }
    worker.finish();
    return true;
  });
}

} // namespace Tcp
} // namespace Network

} // namespace Socks
