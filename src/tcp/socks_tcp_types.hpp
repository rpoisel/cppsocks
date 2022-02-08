#ifndef SOCKS_TCP_TYPES_HPP
#define SOCKS_TCP_TYPES_HPP

#include <array>
#include <cstdint>

namespace Socks
{
using Byte = std::uint8_t;
namespace Network
{
namespace Tcp
{
/* Size of worker thread receive buffer. */
constexpr std::size_t const MAX_SIZE = 40960;
/* Worker thread receive buffer. WebSocket frames are read from the tcp socket to this buffer. */
using MsgBuf = std::array<Byte, MAX_SIZE>;
} // namespace Tcp
} // namespace Network
} // namespace Socks

#endif /* SOCKS_TCP_TYPES_HPP */
