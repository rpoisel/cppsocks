### compiled library
add_library(socks_http STATIC
  socks_http.cpp
  socks_http_handler.cpp
  socks_http_states.cpp
  socks_http_states_ws.cpp
  socks_ws_handler.cpp
)

target_include_directories(socks_http PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(socks_http PUBLIC
  socks_tcp
)

target_link_libraries(socks_http PRIVATE
  digestpp_header_only
  cppcodec_header_only
)

target_compile_definitions(socks_http PRIVATE
  -DSOCKS_INLINE=
)

set_property(TARGET socks_http PROPERTY CXX_STANDARD 11)

### header-only library
add_library(socks_http_header_only INTERFACE)

# SOCKS_INLINE is already INTERFACE defined in socks_tcp_header_only
target_compile_definitions(socks_http_header_only INTERFACE
  -DSOCKS_HEADER_ONLY=1
)

target_include_directories(socks_http_header_only INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(socks_http_header_only INTERFACE
  socks_tcp_header_only
  digestpp_header_only
  cppcodec_header_only
)
