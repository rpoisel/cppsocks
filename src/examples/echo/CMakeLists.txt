add_executable(tcp_echo
  main.cpp
)

target_link_libraries(tcp_echo PRIVATE
  sys_ctx_posix
)
