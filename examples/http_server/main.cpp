#include <socks_http.h>
#include <system_impl.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <cstring>

using Socks::Network::Http::HttpConnection;
using Socks::Network::Http::HttpHandler;
using Socks::Network::Http::RequestInfo;
using Socks::Network::Http::Server;
using Socks::Network::Http::ServerOptions;
using Socks::Network::Http::WSHandlerNull;

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  Server server;

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, "/tmp", ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}