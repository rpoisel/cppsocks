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

class SimpleServerHandler final : public HttpHandler
{
  void do_GET(HttpConnection* connection, RequestInfo const* requestInfo)
  {
    spdlog::info("Requested path: {}", requestInfo->path());

    constexpr char const* const MSG = "Hello, World!\r\n";

    connection->http_200(std::strlen(MSG));
    connection->write(MSG, std::strlen(MSG));
  }
};

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  SimpleServerHandler httpHandler;
  Server server;

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, httpHandler, ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}