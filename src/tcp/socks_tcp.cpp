#include <socks_tcp.h>
#include <socks_tcp_types.h>
#include <socks_tcp_worker.h>

#include <algorithm>
#include <cstring>
#include <list>

namespace Socks
{

namespace Network
{
namespace Tcp
{

using ClientWorkers = std::list<ClientWorker>;

static void cleanup(ClientWorkers& workers);

void Server::serve(Context& context, ServerHandler& handler, ServerOptions const& options)
{
  (void)handler;

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
      workers.emplace_back(std::move(clientSocket), handler, this);
      continue;
    }
    constexpr auto const bye = "No more connections allowed.\n";
    clientSocket->write(bye, std::strlen(bye));
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
