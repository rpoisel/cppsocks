#include <socks_ws_frame.hpp>

#include <catch2/catch.hpp>

TEST_CASE("ntoh<std::uint16_t>", "[classic]")
{
  Socks::Byte buf[] = {0x01, 0x02};
  REQUIRE(Socks::Network::Http::WebSocketFrame::ntoh<std::uint16_t>(&buf[0]) == 0x0102);
}

TEST_CASE("ntoh<std::uint32_t>", "[classic]")
{
  Socks::Byte buf[] = {0x01, 0x02, 0x03, 0x04};
  REQUIRE(Socks::Network::Http::WebSocketFrame::ntoh<std::uint32_t>(&buf[0]) == 0x01020304);
}

TEST_CASE("ntoh<std::uint64_t>", "[classic]")
{
  Socks::Byte buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  REQUIRE(Socks::Network::Http::WebSocketFrame::ntoh<std::uint64_t>(&buf[0]) == 0x0102030405060708);
}

TEST_CASE("hton<std::uint16_t>", "[classic]")
{
  std::uint16_t value = 0x0102;
  Socks::Byte buf[sizeof(value)];
  Socks::Network::Http::WebSocketFrame::hton(&buf[0], value);
  REQUIRE(buf[0] == 0x01);
  REQUIRE(buf[1] == 0x02);
}

TEST_CASE("hton<std::uint32_t>", "[classic]")
{
  std::uint32_t value = 0x01020304;
  Socks::Byte buf[sizeof(value)];
  Socks::Network::Http::WebSocketFrame::hton(&buf[0], value);
  REQUIRE(buf[0] == 0x01);
  REQUIRE(buf[1] == 0x02);
  REQUIRE(buf[2] == 0x03);
  REQUIRE(buf[3] == 0x04);
}

TEST_CASE("hton<std::uint64_t>", "[classic]")
{
  std::uint64_t value = 0x0102030405060708;
  Socks::Byte buf[sizeof(value)];
  Socks::Network::Http::WebSocketFrame::hton(&buf[0], value);
  REQUIRE(buf[0] == 0x01);
  REQUIRE(buf[1] == 0x02);
  REQUIRE(buf[2] == 0x03);
  REQUIRE(buf[3] == 0x04);
  REQUIRE(buf[4] == 0x05);
  REQUIRE(buf[5] == 0x06);
  REQUIRE(buf[6] == 0x07);
  REQUIRE(buf[7] == 0x08);
}
