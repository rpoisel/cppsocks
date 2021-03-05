#include <socks_tcp.hpp>
#include <socks_tcp_types.hpp>
#include <socks_tcp_worker.hpp>

#include <algorithm>
#include <cstring>
#include <list>

#include <string>
#include <vector>


namespace Socks
{

namespace Network
{
namespace Tcp
{

using ClientWorkers = std::list<ClientWorker>;

static void cleanup(ClientWorkers& workers);

SOCKS_INLINE void Server::serve(std::vector<std::string>& clientTypes, Context& context,
                                ServerHandlerFactory& handlerFactory, ServerOptions const& options)
{
  ClientWorkers workers;

  auto listenSocket = context.createListenSocket(options.serverPort);
  while (!System::quitCondition())
  {
    auto clientSocket = listenSocket->accept();
    if (!clientSocket.get())
    {
      continue;
    }
    cleanup(workers);
    if (workers.size() < options.maxClients)
    {
      std::string clientType = std::string("client") + std::to_string(workers.size());
      workers.emplace_back(clientSocket, handlerFactory.createServerHandler(clientSocket, this));
      clientTypes.push_back(clientType);
      continue;
    }
    constexpr auto const bye = "No more connections allowed.\n";
    clientSocket->write(reinterpret_cast<Socks::Byte const*>(bye), std::strlen(bye));
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
