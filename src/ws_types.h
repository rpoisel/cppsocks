#ifndef WS_TYPES_H
#define WS_TYPES_H

#include <array>
#include <cstdint>

namespace WS
{
constexpr std::size_t const MAX_SIZE = 1024;

using Byte = std::uint8_t;
using MsgBuf = std::array<Byte, MAX_SIZE>;
} // namespace WS

#endif // WS_TYPES_H