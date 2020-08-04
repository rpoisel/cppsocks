find_package(Threads)

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
  wslay
 )

target_compile_options(socks_tcp PUBLIC
  -Wall
  -Wextra
 )

