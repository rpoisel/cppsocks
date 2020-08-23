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
  handler->onConnect(&conn);
  loop();
  handler->onDisconnect(&conn);
  socket->close();
  _isActive = false;
}

void ClientWorker::loop()
{
  MsgBuf buf;

  while (!System::quitCondition() && !conn.isClosed())
  {
    if (socket->isReadable())
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
      handler->onReceive(&conn, buf.data(), len);
    }
    if (socket->isWriteable())
    {
      handler->canSend(&conn);
    }
  }
}

} // namespace Tcp
} // namespace Network

} // namespace Socks
