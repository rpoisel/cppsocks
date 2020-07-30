#include <ws_worker.h>

#include <ws_types.h>

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

  for (;;)
  {
    auto len = socket->read(buf);
    if (len <= 0)
    {
      // TODO check for errors
      break;
    }
    handler.onReceive(&conn, buf.data(), len);
  }
}

} // namespace WS