#include <pusher.hpp>

void pusherRunner(ConnMgr& connMgr)
{
  std::stringstream msg;
  for (std::size_t cnt = 0; !Socks::System::quitCondition(); cnt++)
  {
    msg.str("");
    msg.clear();
    msg << cnt << std::endl;
    connMgr.push(reinterpret_cast<Socks::Byte const*>(msg.str().c_str()), msg.str().length() + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
