add_executable(http_simple
  main.cpp
)

target_link_libraries(http_simple PRIVATE
  socks_http
  sys_ctx_posix
)
