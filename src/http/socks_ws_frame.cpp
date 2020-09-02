#include <socks_ws_frame.hpp>

namespace Socks
{
namespace Network
{
namespace Http
{

SOCKS_INLINE WebSocketFrame WebSocketFrame::createConnectionClose(CloseReasonCode reasonCode)
{
  Byte payload_[sizeof(CloseReasonCode)];

  hton(&payload_[0], reasonCode);

  return WebSocketFrame(&payload_[0], sizeof(payload_), true, OPCODE_CONNECTION_CLOSE);
}

SOCKS_INLINE WebSocketFrame WebSocketFrame::createPong(Byte const* buf, std::size_t len)
{
  return WebSocketFrame(buf, len, true, OPCODE_CONNECTION_PONG);
}

SOCKS_INLINE bool WebSocketFrame::decode(Byte const* buf, std::size_t len, WsBuffer& payloadBuf, OpCode* opcode /* output */)
{
  if (len < 2)
  {
    throw std::invalid_argument("WebSocket header is missing.");
  }

  std::size_t frameLen = (buf[1] & 0x7f);
  bool mask = (buf[1] & 0x80) == 0x80;
  *opcode = buf[0] & 0x0f;
  std::size_t payloadOffset = 2;
  // TODO check for correct len (must be > 2 now)
  if (frameLen == 126)
  {
    frameLen = ntoh<std::uint16_t>(&buf[2]);
    payloadOffset += 2;
  }
  else if (frameLen == 127)
  {
    frameLen = ntoh<std::uint64_t>(&buf[2]);
    payloadOffset += 8;
  }
  auto oldPayloadSize = payloadBuf.size();
  payloadBuf.resize(payloadBuf.size() + frameLen);

  if (mask)
  {
    Byte const maskingKey[4]{buf[payloadOffset], buf[payloadOffset + 1], buf[payloadOffset + 2], buf[payloadOffset + 3]};
    for (std::size_t cnt = 0; cnt < frameLen; cnt++)
    {
      payloadBuf[oldPayloadSize + cnt] = (buf[payloadOffset + 4 + cnt] ^ maskingKey[cnt % 4]);
    }
  }
  else
  {
    std::memcpy(payloadBuf.data(), buf + payloadOffset, frameLen);
  }

  return (buf[0] & 0x80) == 0x80; /* value of FIN bit */
}

SOCKS_INLINE std::size_t WebSocketFrame::payloadLength() const
{
  auto frameLen = (data_[1] & 0x7f);
  if (frameLen == 127)
  {
    throw std::runtime_error("Length > UINT16_MAX not supported yet.");
  }
  if (frameLen == 126)
  {
    return ntoh<std::uint16_t>(&data_[2]);
  }
  return frameLen;
}

SOCKS_INLINE WebSocketFrame::WebSocketFrame(Byte const* payload, std::size_t payloadLength, bool fin)
    : WebSocketFrame(payload, payloadLength, fin, OPCODE_BINARY)
{
}

SOCKS_INLINE WebSocketFrame::WebSocketFrame(char const* payload)
    : WebSocketFrame(reinterpret_cast<Byte const*>(payload), std::strlen(payload), true, OPCODE_TEXT)
{
}

SOCKS_INLINE WebSocketFrame::WebSocketFrame(Byte const* payload, std::size_t payloadLength, bool fin, OpCode opcode)
    : data_(2)
{
  std::size_t headerSize = 2;
  if (fin)
  {
    data_[0] = 0x80;
  }
  if (opcode > 15) // TODO: do not use magic number
  {
    throw std::invalid_argument("WebSocket opcode must not be bigger than 15.");
  }
  data_[0] |= opcode;

  if (payloadLength < 126)
  {
    data_[1] |= payloadLength;
  }
  else if (payloadLength <= UINT16_MAX)
  {
    data_[1] |= 126;
    headerSize += 2;
    data_.resize(headerSize);
    hton(&data_[2], static_cast<std::uint16_t>(payloadLength));
  }
  else
  {
    throw std::invalid_argument("Payload length > UINT16_MAX currently not supported.");
  }


  data_.resize(headerSize + payloadLength);
  std::memcpy(data_.data() + headerSize, payload, payloadLength);
}

} // namespace Http
} // namespace Network
} // namespace Socks
