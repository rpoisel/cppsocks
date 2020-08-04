#include <system_context.h>
#include <ws_types.h>
#include <ws_worker.h>

namespace WS
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
    if (len == Network::TcpSocket::NUM_EOF)
    {
      break;
    }
    if (len == Network::TcpSocket::NUM_CONTINUE)
    {
      continue;
    }
    handler.onReceive(&conn, buf.data(), len);
  }
}

} // namespace WS