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

template <typename T>
static T ntoh(Byte const* data)
{
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
  for (std::size_t cnt = 0; cnt < sizeof(T); cnt++)
  {
    data[cnt] = ((value >> ((sizeof(T) - 1 - cnt) * 8)) & 0xff);
  }
}

namespace Http
{

class WebSocketFrame
{
  public:
  using CloseReasonCode = std::uint16_t;
  using OpCode = std::uint8_t;

  static constexpr OpCode const OPCODE_CONTINUATION = 0x00;
  static constexpr OpCode const OPCODE_TEXT = 0x01;
  static constexpr OpCode const OPCODE_BINARY = 0x02;
  static constexpr OpCode const OPCODE_CONNECTION_CLOSE = 0x08;
  static constexpr OpCode const OPCODE_CONNECTION_PING = 0x09;
  static constexpr OpCode const OPCODE_CONNECTION_PONG = 0x0A;

  static WebSocketFrame createConnectionClose(CloseReasonCode reasonCode = 0);
  static WebSocketFrame createPong(Byte const* buf, std::size_t len);
  static bool decode(Byte const* buf, std::size_t len, WsBuffer& payloadBuf, OpCode* opcode /* output */);

  explicit WebSocketFrame(Byte const* payload, std::size_t payloadLength, bool fin, OpCode opcode);
  explicit WebSocketFrame(Byte const* payload, std::size_t payloadLength, bool fin);
  explicit WebSocketFrame(char const* payload);

  Byte const* data() const { return data_.data(); }
  std::size_t length() const { return data_.size(); }
  Byte const* payload() const { return payloadLength() < 126 ? &data_[2] : &data_[4]; }
  std::size_t payloadLength() const;
  bool fin() const { return (data_[0] & 0x80) == 0x80; }
  bool mask() const { return (data_[1] & 0x80) == 0x80; }

  private:
  WsBuffer data_;
};

} // namespace Http
} // namespace Network
} // namespace Socks

#if SOCKS_HEADER_ONLY == 1
#include <socks_ws_frame.cpp>
#endif /* SOCKS_HEADER_ONLY */

#endif /* SOCKS_WS_FRAME_HPP */
