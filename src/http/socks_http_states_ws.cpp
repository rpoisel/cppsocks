#include <socks_http_states_ws.h>
#include <socks_ws_types.h>

#include <cppcodec/base64_rfc4648.hpp>
#include <digestpp.hpp>

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
static std::string createAcceptKey(std::string const& client_key)
{
  using base64 = cppcodec::base64_rfc4648;

  Byte sha1buf[160 / 8];
  static constexpr char const WS_GUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  digestpp::sha1().absorb(client_key.substr(0, 24) + WS_GUID).digest(sha1buf, sizeof(sha1buf));
  return base64::encode(sha1buf, sizeof(sha1buf));
}

void HttpWsState::onEnter()
{
  std::stringstream response;
  response << "HTTP/1.1 101 Switching Protocols\r\n"
              "Upgrade: websocket\r\n"
              "Connection: Upgrade\r\n"
              "Sec-WebSocket-Version: 13\r\n";
  if (!handler->subprotocol().empty())
  {
    response << "Sec-WebSocket-Protocol: " << handler->subprotocol() << "\r\n";
  }
  response << "Sec-WebSocket-Accept: " << createAcceptKey(fsm->requestInfo().headers().at("Sec-WebSocket-Key"))
           << "\r\n\r\n";
  fsm->tcpConnection()->send(response.str().c_str(), response.str().length());
  handler->onConnect();
}

void HttpWsState::onReceive(Byte const* buf, std::size_t len)
{
  Byte payload[1024];
  auto payloadLength = sizeof(payload) / sizeof(payload[0]);
  try
  {
    std::uint8_t opcode;
    // TODO: actually, data has to be buffered until fin is received
    auto fin = WebSocketFrame::decode(buf, len, payload, &payloadLength, &opcode);
    if (fin)
    {
      handler->onData(payload, payloadLength);
    }
  }
  catch (std::invalid_argument& exc)
  {
    (void)exc;
  }
}

void HttpWsState::onDisconnect() { handler->onDisconnect(); }

} // namespace Http
} // namespace Network
} // namespace Socks
