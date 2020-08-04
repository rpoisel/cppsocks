#include <socks_system_context.h>
#include <socks_tcp_types.h>
#include <socks_tcp_worker.h>

namespace Socks
{

namespace Network
{
namespace Tcp
{

void ClientWorker::run()
{
  handler.onConnect(&conn);
  loop();
  handler.onDisconnect(&conn);
  _isActive = false;
}

void ClientWorker::loop()
{
  MsgBuf buf;

  while (!System::quitCondition())
  {
    auto len = socket->read(buf);
    if (len == Socket::NUM_EOF)
    {
      break;
    }
    if (len == Socket::NUM_CONTINUE)
    {
      continue;
    }
    handler.onReceive(&conn, buf.data(), len);
  }
}

} // namespace Tcp
} // namespace Network

} // namespace Socks
