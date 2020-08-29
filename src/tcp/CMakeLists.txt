find_package(Threads)

### compiled library
add_library(socks_tcp STATIC
  socks_tcp.cpp
  socks_tcp_worker.cpp
)

target_include_directories(socks_tcp PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(socks_tcp PUBLIC
  Threads::Threads
  spdlog
 )

target_compile_definitions(socks_tcp PRIVATE
  -DSOCKS_INLINE=
)

target_compile_options(socks_tcp PUBLIC
  -Wall
  -Wextra
 )

### header-only library
add_library(socks_tcp_header_only INTERFACE)

target_compile_definitions(socks_tcp_header_only INTERFACE
  -DSOCKS_HEADER_ONLY=1
  -DSOCKS_INLINE=inline
)

target_include_directories(socks_tcp_header_only INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(socks_tcp_header_only INTERFACE
  Threads::Threads
  spdlog_header_only
)
