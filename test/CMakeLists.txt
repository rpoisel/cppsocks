add_subdirectory(external)

add_library(TestMain STATIC
  test_main.cpp
)

target_link_libraries(TestMain PUBLIC Catch2::Catch2)

### Byte-Order
add_executable(test_byteorder_app
  test_byteorder.cpp
)

target_link_libraries(test_byteorder_app PRIVATE
  TestMain
  socks_http_header_only
)

add_test(NAME test_byteorder COMMAND test_byteorder_app)

### WebSocketFrame
add_executable(test_ws_frame_app
  test_ws_frame.cpp
)

target_link_libraries(test_ws_frame_app PRIVATE
  TestMain
  socks_http
)

add_test(NAME test_ws_frame COMMAND test_ws_frame_app)

