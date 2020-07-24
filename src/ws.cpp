#include <ws.h>
#include <ws_client.h>

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
      workers.emplace_back(std::move(clientSocket));
      continue;
    }
    constexpr char const* bye = "No more connections allowed.";
    clientSocket->write(bye, std::strlen(bye));
  }
}

static void cleanup(ClientWorkers& workers)
{
  workers.remove_if([](ClientWorker& worker) {
    if (worker.isActive())
    {
      return true;
    }
    worker.finish();
    return false;
  });
}

} // namespace WS
