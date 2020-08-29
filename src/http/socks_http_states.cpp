#include <socks_http_states.hpp>
#include <socks_http_states_ws.hpp>

#include <array>
#include <exception>
#include <sstream>
#include <stdexcept>

namespace Socks
{
namespace Network
{
namespace Http
{
SOCKS_INLINE void HttpInitState::onReceive(Byte const* buf, std::size_t len)
{
  constexpr std::array<char, 4> TERMINATOR = {'\r', '\n', '\r', '\n'};
  try
  {
    // TODO check bounds and available buffer sizes
    std::memcpy(recvBuf.data() + numRecv, buf, len);
    numRecv += len;
    if (numRecv < recvBuf.size() &&
        std::memcmp(recvBuf.data() + numRecv - TERMINATOR.size(), TERMINATOR.data(), TERMINATOR.size()))
    {
      return;
    }
    recvBuf[recvBuf.size() - 1] = '\0';

    requestInfo.parse(recvBuf.data(), numRecv);
    spdlog::info("Request Type: {}, Path: {}", fsm->requestInfo().requestType() == RequestType::GET ? "GET" : "GET_WS",
                 fsm->requestInfo().path());
    if (fsm->requestInfo().requestType() == RequestType::GET)
    {
      fsm->setNextState(std::unique_ptr<HttpState>(new HttpGetState(fsm)));
      return;
    }
    else if (fsm->requestInfo().requestType() == RequestType::GET_WS)
    {
      fsm->setNextState(std::unique_ptr<HttpState>(new HttpWsState(fsm)));
      return;
    }
  }
  catch (const std::exception& exc)
  {
    spdlog::error("HTTP request cannot be parsed completely ({})", exc.what());
  }

  fsm->tcpConnection()->close();
}

SOCKS_INLINE void HttpGetState::onEnter()
{
  // Plain HTTP is a stateless protocoll, so the httpConnection can be created ad-hoc
  Socks::Network::Http::HttpConnection httpConnection(fsm->tcpConnection());
  handler->do_GET(&httpConnection, fsm->requestInfo());
  fsm->tcpConnection()->close();
}

SOCKS_INLINE void HttpGetState::onReceive(Byte const* buf, std::size_t len)
{
  (void)buf;
  (void)len;
}

} // namespace Http
} // namespace Network
} // namespace Socks
