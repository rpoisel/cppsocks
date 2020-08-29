#include <socks_system_context.hpp>
#include <socks_tcp_types.hpp>
#include <socks_tcp_worker.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <exception>

namespace Socks
{

namespace Network
{
namespace Tcp
{

void ClientWorker::run()
{
  try
  {
    handler->onConnect();
    loop();
    handler->onDisconnect();
    socket->close();
  }
  catch (std::exception& exc)
  {
    spdlog::error("{}", exc.what());
  }
  _isActive = false;
}

void ClientWorker::loop()
{
  MsgBuf buf;

  while (!System::quitCondition() && !handler->connection()->isClosed())
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
      handler->onReceive(buf.data(), len);
    }
  }
}

} // namespace Tcp
} // namespace Network

} // namespace Socks
