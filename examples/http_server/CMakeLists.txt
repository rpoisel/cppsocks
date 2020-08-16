add_executable(http_server
  main.cpp
)

target_link_libraries(http_server PRIVATE
  socks_http
  sys_ctx_posix
)
