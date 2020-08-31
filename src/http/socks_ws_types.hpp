#ifndef SOCKS_WS_TYPES_H
#define SOCKS_WS_TYPES_H

#include <socks_tcp_types.hpp>

#include <vector>

namespace Socks
{
namespace Network
{
namespace Http
{

using WsBuffer = std::vector<Socks::Byte>;

} // namespace Http
} // namespace Network
} // namespace Socks

#endif /* SOCKS_WS_TYPES_H */
