#include <handler.hpp>

void WsPushHandler::onConnect()
{
  spdlog::info("WebSocket connect.");
  connMgr.addElem(connection());
}
void WsPushHandler::onDisconnect()
{
  connMgr.delElem(connection());
  spdlog::info("WebSocket disconnect.");
}
void WsPushHandler::onData(Socks::Byte const* buf, std::size_t len) { connection()->send(buf, len); }
