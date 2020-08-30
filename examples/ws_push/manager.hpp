#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <socks_ws_types.hpp>

#include <algorithm>
#include <cstddef>
#include <mutex>
#include <set>

template <typename C>
class ElemMgr final
{
  public:
  void addElem(C* elem)
  {
    std::unique_lock<std::mutex> lk(mtx);
    elements.insert(elem);
  }

  void delElem(C* elem)
  {
    std::unique_lock<std::mutex> lk(mtx);
    elements.erase(elem);
  }

  void push(Socks::Byte const* buf, std::size_t len)
  {
    std::unique_lock<std::mutex> lk(mtx);
    std::for_each(elements.begin(), elements.end(),
                  [&](C* connection) { connection->send(buf, len, Socks::Network::Http::WebSocketFrame::OPCODE_TEXT); });
  }

  private:
  std::set<C*> elements;
  std::mutex mtx;
};

#endif /* MANAGER_HPP */
