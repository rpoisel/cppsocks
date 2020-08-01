#include <ws.h>
#include <ws_types.h>
#include <ws_worker.h>

#include <cstring>
#include <list>

namespace WS
{

using ClientWorkers = std::list<ClientWorker>;

static void cleanup(ClientWorkers& workers);

void Server::serve(SystemContext& systemContext, ServerHandler& handler, ServerOptions const& options)
{
  (void)handler;

  ClientWorkers workers;

  auto listenSocket = systemContext.createListenSocket(options.serverPort);
  for (;;)
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

} // namespace WS
