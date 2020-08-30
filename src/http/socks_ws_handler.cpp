#include <socks_ws_handler.hpp>
#include <socks_ws_types.hpp>

namespace Socks
{
namespace Network
{
namespace Http
{
SOCKS_INLINE std::size_t WsConnection::send(Byte const* buf, std::size_t len, std::uint8_t opcode)
{
  auto response = WebSocketFrame::encode(buf, len, true, opcode);
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
