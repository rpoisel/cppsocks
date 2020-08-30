#ifndef SOCKS_WS_TYPES_H
#define SOCKS_WS_TYPES_H

#include <socks_tcp_types.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace Socks
{
namespace Network
{
namespace Http
{
class WebSocketFrame
{
  public:
  static constexpr std::uint8_t const OPCODE_TEXT = 0x01;
  static constexpr std::uint8_t const OPCODE_BINARY = 0x02;
  static constexpr std::uint8_t const OPCODE_CONNECTION_CLOSE = 0x08;
  static constexpr std::uint8_t const OPCODE_CONNECTION_PING = 0x09;
  static constexpr std::uint8_t const OPCODE_CONNECTION_PONG = 0x0A;

  static WebSocketFrame encode(Byte const* payload_, std::size_t payloadLength_, bool fin)
  {
    return WebSocketFrame(fin, OPCODE_BINARY, payload_, payloadLength_);
  }

  static WebSocketFrame encode(Byte const* payload_, std::size_t payloadLength_, bool fin, std::uint8_t opcode)
  {
    return WebSocketFrame(fin, opcode, payload_, payloadLength_);
  }

  static WebSocketFrame encode(char const* payload_, std::size_t payloadLength_, bool fin)
  {
    return WebSocketFrame(fin, OPCODE_TEXT, reinterpret_cast<Byte const*>(payload_), payloadLength_);
  }

  using CloseReasonCode = std::uint16_t;
  static WebSocketFrame createConnectionClose(CloseReasonCode reasonCode = 0)
  {
    Byte payload_[2];

    payload_[0] = (reasonCode & 0x00ff);
    payload_[1] = (reasonCode >> 8);

    return WebSocketFrame(true, OPCODE_CONNECTION_CLOSE, &payload_[0], sizeof(payload_));
  }

  static WebSocketFrame createPong(Byte const* buf, std::size_t len)
  {
    return WebSocketFrame(true, OPCODE_CONNECTION_PONG, buf, len);
  }

  static bool decode(Byte const* buf, std::uint8_t len, Byte* payload /* in */, std::size_t* payloadLength /* inout */,
                     std::uint8_t* opcode /* output */)
  {
    (void)len;

    if (len < 2)
    {
      throw std::invalid_argument("WebSocket header is missing.");
    }

    std::uint8_t frameLen = (buf[1] & 0x7f);
    if (frameLen > *payloadLength)
    {
      throw std::invalid_argument("Given payload buffer is too small.");
    }
    *payloadLength = frameLen;
    bool mask = (buf[1] & 0x80) == 0x80;

    if (frameLen > 125)
    {
      throw std::runtime_error("WebSockets with a length > 125 bytes are not supported yet.");
    }
    if ((!mask && len != frameLen + 2) || (mask && len != frameLen + 6))
    {
      throw std::runtime_error("Illegal frame length.");
    }

    *opcode = buf[0] & 0x0f;
    if (mask)
    {
      Byte const maskingKey[4]{buf[2], buf[3], buf[4], buf[5]};
      for (std::size_t cnt = 0; cnt < frameLen; cnt++)
      {
        payload[cnt] = (buf[6 + cnt] ^ maskingKey[cnt % 4]);
      }
    }
    else
    {
      std::memcpy(payload, buf + 2, frameLen);
    }

    return (buf[0] & 0x80) == 0x80; /* value of FIN bit */
  }

  Byte const* data() const { return data_.data(); }
  std::size_t length() const { return dataLen; }

  Byte const* payload() const { return data_.data() + 2; }
  std::size_t payloadLength() const { return dataLen - 2; }

  private:
  WebSocketFrame(bool fin, std::uint8_t opcode, Byte const* payload, std::size_t payloadLength)
      : data_{0}, dataLen{payloadLength + 2}
  {
    if (fin)
    {
      data_[0] |= 0x80;
    }
    if (opcode > 15)
    {
      throw std::invalid_argument("WebSocket opcode must not be bigger than 15.");
    }
    data_[0] |= opcode;

    if (payloadLength > 125)
    {
      throw std::invalid_argument("Payload length > 125 not supported.");
    }
    data_[1] |= payloadLength;

    std::memcpy(data_.data() + 2, payload, payloadLength);
  }

  std::array<Byte, 128> data_;
  std::size_t dataLen;
};

} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_WS_TYPES_H */
