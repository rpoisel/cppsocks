#include <socks_http_states_ws.hpp>
#include <socks_ws_frame.hpp>

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

SOCKS_INLINE void HttpWsState::onEnter()
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

SOCKS_INLINE void HttpWsState::onReceive(Byte const* buf, std::size_t len)
{
  try
  {
    std::uint8_t opcode_;
    bool start = inBuf.size() == 0;
    auto fin = WebSocketFrame::decode(buf, len, inBuf, &opcode_);
    if (start)
    {
      opcode = opcode_;
    }
    if (!fin)
    {
      // just received data
      return;
    }

    switch (opcode)
    {
    case WebSocketFrame::OPCODE_CONTINUATION:
      throw std::runtime_error("WebSocket opcode must not be 0 when FIN bit is set.");
      break;
    case WebSocketFrame::OPCODE_CONNECTION_CLOSE:
      handler->connection()->close();
      break;
    case WebSocketFrame::OPCODE_CONNECTION_PING:
    {
      auto response = WebSocketFrame::createPong(inBuf.data(), inBuf.size());
      fsm->tcpConnection()->send(response.payload(), response.payloadLength());
      break;
    }
    case WebSocketFrame::OPCODE_BINARY:
      handler->onData(inBuf.data(), inBuf.size());
      break;
    case WebSocketFrame::OPCODE_TEXT:
      handler->onText(reinterpret_cast<char const*>(inBuf.data()), inBuf.size());
      break;
    default:
      break;
    }
    inBuf.clear();
  }
  catch (std::invalid_argument& exc)
  {
    (void)exc;
  }
}

SOCKS_INLINE void HttpWsState::onDisconnect() { handler->onDisconnect(); }

} // namespace Http
} // namespace Network
} // namespace Socks
