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
  using CloseReasonCode = std::uint16_t;

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

  static WebSocketFrame encode(Byte const* payload_, std::size_t payloadLength_, bool fin);
  static WebSocketFrame encode(Byte const* payload_, std::size_t payloadLength_, bool fin, std::uint8_t opcode);
  static WebSocketFrame encode(char const* payload_);
  static WebSocketFrame createConnectionClose(CloseReasonCode reasonCode = 0);
  static WebSocketFrame createPong(Byte const* buf, std::size_t len);
  static bool decode(Byte const* buf, std::size_t len, WsBuffer& payloadBuf, std::uint8_t* opcode /* output */);

  Byte const* data() const { return data_.data(); }
  std::size_t length() const { return data_.size(); }
  Byte const* payload() const { return payloadLength() < 126 ? &data_[2] : &data_[4]; }
  std::size_t payloadLength() const;
  bool fin() const { return (data_[0] & 0x80) == 0x80; }
  bool mask() const { return (data_[1] & 0x80) == 0x80; }

  private:
  WebSocketFrame(bool fin, std::uint8_t opcode, Byte const* payload, std::size_t payloadLength);

  WsBuffer data_;
};

} // namespace Http
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_ws_frame.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_WS_FRAME_HPP */
