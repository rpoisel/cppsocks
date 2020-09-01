#ifndef SOCKS_WS_FRAME_HPP
#define SOCKS_WS_FRAME_HPP

#include <socks_tcp_types.hpp>
#include <socks_ws_types.hpp>

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
  static constexpr std::uint8_t const OPCODE_CONTINUATION = 0x00;
  static constexpr std::uint8_t const OPCODE_TEXT = 0x01;
  static constexpr std::uint8_t const OPCODE_BINARY = 0x02;
  static constexpr std::uint8_t const OPCODE_CONNECTION_CLOSE = 0x08;
  static constexpr std::uint8_t const OPCODE_CONNECTION_PING = 0x09;
  static constexpr std::uint8_t const OPCODE_CONNECTION_PONG = 0x0A;

  template <typename T>
  static T ntoh(Byte const* data)
  {
    // TODO check if host works natively with network byte-order
    T result = 0;
    for (std::size_t cnt = 0; cnt < sizeof(T); cnt++)
    {
      result += (static_cast<T>(data[cnt]) << ((sizeof(T) - cnt - 1) * 8));
    }
    return result;
  }

  template <typename T>
  static void hton(Byte* data, T value)
  {
    // TODO check if host works natively with network byte-order
    for (std::size_t cnt = 0; cnt < sizeof(T); cnt++)
    {
      data[cnt] = ((value >> ((sizeof(T) - 1 - cnt) * 8)) & 0xff);
    }
  }

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
    Byte payload_[sizeof(CloseReasonCode)];

    hton(&payload_[0], reasonCode);

    return WebSocketFrame(true, OPCODE_CONNECTION_CLOSE, &payload_[0], sizeof(payload_));
  }

  static WebSocketFrame createPong(Byte const* buf, std::size_t len)
  {
    return WebSocketFrame(true, OPCODE_CONNECTION_PONG, buf, len);
  }

  static bool decode(Byte const* buf, std::size_t len, WsBuffer& payloadBuf, std::uint8_t* opcode /* output */)
  {
    (void)len;

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

  Byte const* data() const { return data_.data(); }
  std::size_t length() const { return data_.size(); }

  Byte const* payload() const { return payloadLength() < 126 ? &data_[2] : &data_[4]; }
  std::size_t payloadLength() const
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

  bool fin() const { return (data_[0] & 0x80) == 0x80; }

  private:
  WebSocketFrame(bool fin, std::uint8_t opcode, Byte const* payload, std::size_t payloadLength) : data_{0, 0}
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

  WsBuffer data_;
};

} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_WS_FRAME_HPP */
