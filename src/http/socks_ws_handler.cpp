#include <socks_ws_handler.h>
#include <socks_ws_types.h>

namespace Socks
{
namespace Network
{
namespace Http
{
std::size_t WsConnection::send(Byte const* buf, std::size_t len)
{
  auto response = WebSocketFrame::encode(buf, len, true, WebSocketFrame::OPCODE_BINARY);
  tcpConnection->send(response.data(), response.length());
  return len; // TODO fix this
}
} // namespace Http
} // namespace Network
} // namespace Socks
