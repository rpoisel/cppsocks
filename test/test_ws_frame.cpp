#include <socks_ws_frame.hpp>

#include <catch2/catch.hpp>

TEST_CASE("WebSocketFrame::decode", "[classic]")
{
  Socks::Byte payload[] = {0x01, 0x02};
  auto frame = Socks::Network::Http::WebSocketFrame::createPong(payload, sizeof(payload) / sizeof(payload[0]));
  REQUIRE(frame.length() == 4);
  REQUIRE(frame.data()[0] == 0x8A);
  REQUIRE(frame.data()[1] == 0x02);
  REQUIRE(frame.data()[2] == 0x01);
  REQUIRE(frame.data()[3] == 0x02);
  REQUIRE(frame.payloadLength() == 2);
  REQUIRE(frame.payload()[0] == 0x01);
  REQUIRE(frame.payload()[1] == 0x02);
  REQUIRE(frame.fin() == true);
}
