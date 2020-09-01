#include <pusher.hpp>

void pusherRunner(ConnMgr& connMgr)
{
  std::stringstream msg;
  for (std::size_t cnt = 0; !Socks::System::quitCondition(); cnt++)
  {
    msg.str("");
    msg.clear();
    msg << cnt << std::endl;
    connMgr.push(msg.str().c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
