#ifndef SOCKS_HTTP_STATES_WS_HPP
#define SOCKS_HTTP_STATES_WS_HPP

#include <socks_http_states.hpp>
#include <socks_ws_handler.hpp>

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

#if SOCKS_HEADER_ONLY == 1
#include <socks_http_states_ws.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_HTTP_STATES_WS_HPP */
