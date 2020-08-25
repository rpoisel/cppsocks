#ifndef SOCKS_HTTP_STATES_WS_H
#define SOCKS_HTTP_STATES_WS_H

#include <socks_http_states.h>
#include <socks_ws_handler.h>

namespace Socks
{
namespace Network
{
namespace Http
{
class HttpWsState final : public HttpState
{
  public:
  explicit HttpWsState(HttpStateContext* fsm)
      : HttpState{fsm}, handler{fsm->wsHandlerFactory().createWsHandler(fsm->tcpConnection())}
  {
  }
  void onEnter() override;
  void onReceive(Byte const* buf, std::size_t len) override;
  void onDisconnect() override;

  private:
  WsHandlerInstance handler;
};
} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_HTTP_STATES_WS_H */
