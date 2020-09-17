<p align="center">
  <a href="https://travis-ci.com/rpoisel/cppsocks">
    <img src="https://api.travis-ci.com/rpoisel/cppsocks.svg?branch=master">
  </a>
  <a href="https://codecov.io/gh/rpoisel/cppsocks">
    <img src="https://codecov.io/gh/rpoisel/cppsocks/branch/master/graph/badge.svg" />
  </a>
</p>

# cppsocks

C++11 framework that allows for easy WebSockets (RFC 6455) based communication.

This is a framework with minimal dependencies (spdlog, cppcodec, and digestpp to be more specific). Basic socket interaction is abstracted away, all protocol stuff is implemented by this library. In addition to implementing WebSockets communications, this framework can also be used to implement more low-level functionality such as TCP communications.

## Using

The framework itself can be built as a library but it can also be used as a header-only library.

In order to make the platform specific parts (the so called "system context") work, a naming scheme has to be followed for the include files expected by the framework itself.

## Examples

The following examples go down the different layers of the protocol stack.

### WebSockets

The following example shows how to implement a WebSockets echo server using this library:

```` C++
#include <socks_http.hpp>
#include <socks_http_handler.hpp>
#include <socks_ws_handler.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <cstring>

using Posix::Network::Tcp::ContextImpl;
using Socks::Network::Http::HttpHandlerNullFactory;
using Socks::Network::Http::Server;
using Socks::Network::Http::ServerOptions;
using Socks::Network::Http::WsHandler;
using Socks::Network::Http::WsHandlerFactory;
using Socks::Network::Http::WsHandlerFactoryDefault;
using Socks::Network::Http::WsHandlerInstance;
using Socks::Network::Tcp::Connection;

class WsEchoHandler final : public WsHandler
{
  public:
  WsEchoHandler(Connection* tcpConnection) : WsHandler(tcpConnection) {}
  void onConnect() override { spdlog::info("WebSocket connect."); }
  void onText(char const* buf) override { connection()->send(buf); }
  void onData(Socks::Byte const* buf, std::size_t len) override { connection()->send(buf, len); }
  void onDisconnect() override { spdlog::info("WebSocket disconnect."); }
};

int main()
{
  ContextImpl systemContextImpl;
  HttpHandlerNullFactory httpHandlerFactory;
  class WsHandlerFactoryDefault<WsEchoHandler> wsHandlerFactory;

  Socks::System::initQuitCondition();

  try
  {
    Server::serve(systemContextImpl, httpHandlerFactory, wsHandlerFactory, ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
````

### HTTP

A HTTP server serving static web pages located in a directory can be implemented as follows:

```` C++
#include <socks_http.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <cstring>

using Socks::Network::Http::HttpConnection;
using Socks::Network::Http::HttpHandler;
using Socks::Network::Http::RequestInfo;
using Socks::Network::Http::Server;
using Socks::Network::Http::ServerOptions;

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  Socks::System::initQuitCondition();

  try
  {
    Server::serve(systemContextImpl, "/tmp", ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
````

### TCP

The last sample code shows how to implement a TCP echo server:

```` C++
#include <socks_tcp.hpp>
#include <socks_tcp_types.hpp>
#include <system_impl.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>
#include <set>

using Socks::Byte;
using Socks::Network::Tcp::Connection;
using Socks::Network::Tcp::Server;
using Socks::Network::Tcp::ServerHandler;
using Socks::Network::Tcp::ServerHandlerFactory;
using Socks::Network::Tcp::ServerHandlerInstance;

class EchoHandler final : public ServerHandler
{
  public:
  explicit EchoHandler(Socks::Network::Tcp::SocketInstance socket, Server* server) : ServerHandler(socket, server) {}

  void onConnect() override { spdlog::info("Connection established."); }
  void onDisconnect() override { spdlog::info("Connection closed."); }
  void onReceive(Byte const* buf, std::size_t len) override
  {
    spdlog::info("Got message with length {} bytes.", len);
    connection()->send(buf, len);
    spdlog::info("Sent message with length {} bytes.", len);
  }
};

class EchoHandlerFactory final : public ServerHandlerFactory
{
  public:
  ServerHandlerInstance createServerHandler(Socks::Network::Tcp::SocketInstance socket, Server* server) override
  {
    return ServerHandlerInstance(new EchoHandler(socket, server));
  }
};

int main()
{
  Posix::Network::Tcp::ContextImpl systemContextImpl;
  EchoHandlerFactory echoHandlerFactory;
  Socks::Network::Tcp::Server server;

  Socks::System::initQuitCondition();

  try
  {
    server.serve(systemContextImpl, echoHandlerFactory, Socks::Network::Tcp::ServerOptions());
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
````
