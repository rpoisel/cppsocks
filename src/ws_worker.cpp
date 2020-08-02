#include <system_context.h>
#include <ws_types.h>
#include <ws_worker.h>

namespace WS
{

void ClientWorker::run()
{
  handler.onConnect(&conn);
  if (httpHandshake())
  {
    return;
  }
  wsLoop();
  handler.onDisconnect(&conn);
  _isActive = false;
}

bool ClientWorker::httpHandshake() { return false; }

void ClientWorker::wsLoop()
{
  MsgBuf buf;

  while (!System::quitCondition())
  {
    auto len = socket->read(buf);
    if (len == 0)
    {
      break;
    }
    if (len == -1)
    {
      continue;
    }
    handler.onReceive(&conn, buf.data(), len);
  }
}

} // namespace WS