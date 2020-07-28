#ifndef WS_H
#define WS_H

#include <system_context.h>
#include <ws_error.h>
#include <ws_handler.h>
#include <ws_options.h>

namespace WS
{

class Server
{
  public:
  Server() = default;
  virtual ~Server() = default;
  void serve(SystemContext& systemContext, ServerHandler& handler, ServerOptions const& options);

  private:
  Server(Server&) = delete;
  Server& operator=(Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;
};

} // namespace WS

#endif /* WS_H */
