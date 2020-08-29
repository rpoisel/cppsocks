#ifndef SOCKS_WS_H
#define SOCKS_WS_H

#include <socks_ws_handler.hpp>

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
  static constexpr std::uint8_t const OPCODE_BINARY = 0x02;

  static WebSocketFrame encode(Byte const* payload, std::size_t payloadLength, bool fin, std::uint8_t opcode)
  {
    return WebSocketFrame(fin, opcode, payload, payloadLength);
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

#endif /* SOCKS_WS_H */
