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
    if (!listenSocket->waitForConnection())
    {
      continue;
    }
    auto clientSocket = listenSocket->accept();
    cleanup(workers);
    if (workers.size() < options.maxClients)
    {
      ClientWorker clientWorker(std::move(clientSocket), this);
      clientWorker.start();
      workers.push_back(std::move(clientWorker));
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
