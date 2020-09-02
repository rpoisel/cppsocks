#include <socks_ws_frame.hpp>
#include <socks_ws_handler.hpp>

namespace Socks
{
namespace Network
{
namespace Http
{
SOCKS_INLINE std::size_t WsConnection::send(Byte const* buf, std::size_t len)
{
  auto response = WebSocketFrame(buf, len, true, WebSocketFrame::OPCODE_BINARY);
  tcpConnection->send(response.data(), response.length());
  return response.payloadLength();
}

SOCKS_INLINE std::size_t WsConnection::send(char const* buf)
{
  auto response = WebSocketFrame(buf);
  tcpConnection->send(response.data(), response.length());
  return response.payloadLength();
}

SOCKS_INLINE void WsConnection::close()
{
  auto message = WebSocketFrame::createConnectionClose();
  tcpConnection->send(message.data(), message.length());
  tcpConnection->close();
}
} // namespace Http
} // namespace Network
} // namespace Socks
