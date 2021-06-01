#ifndef SOCKS_TCP_HPP
#define SOCKS_TCP_HPP

#include <socks_system_context.hpp>
#include <socks_tcp_error.hpp>
#include <socks_tcp_handler.hpp>
#include <socks_tcp_options.hpp>

#include <string>
#include <vector>

namespace Socks
{

namespace Network
{
namespace Tcp
{

class Server final
{
  public:
  Server() = default;
  void serve(std::vector<std::string>& clientTypes, Context& context, ServerHandlerFactory& handlerFactory,
             ServerOptions const& options = ServerOptions());

  private:
  Server(Server&) = delete;
  Server& operator=(Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};

} // namespace Tcp
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_tcp.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_TCP_HPP */
