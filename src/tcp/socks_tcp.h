#ifndef SOCKS_TCP_H
#define SOCKS_TCP_H

#include <socks_system_context.h>
#include <socks_tcp_error.h>
#include <socks_tcp_handler.h>
#include <socks_tcp_options.h>

namespace Socks
{

namespace Network
{
namespace Tcp
{

class Server
{
  public:
  Server() = default;
  virtual ~Server() = default;
  void serve(Context& context, ServerHandler& handler, ServerOptions const& options);

  private:
  Server(Server&) = delete;
  Server& operator=(Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};

} // namespace Tcp
} // namespace Network

} // namespace Socks

#endif /* SOCKS_TCP_H */
