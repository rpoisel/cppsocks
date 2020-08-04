add_executable(tcp_push
  main.cpp
)

target_link_libraries(tcp_push PRIVATE
  sys_ctx_posix
)
