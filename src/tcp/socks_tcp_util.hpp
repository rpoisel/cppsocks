#ifndef SOCKS_TCP_UTIL_HPP
#define SOCKS_TCP_UTIL_HPP

#include <socks_tcp_types.hpp>

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

} // namespace Network
} // namespace Socks

#endif /* SOCKS_TCP_UTIL_HPP */
