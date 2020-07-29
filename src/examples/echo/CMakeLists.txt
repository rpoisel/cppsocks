add_executable(ws_echo
  main.cpp
)

target_link_libraries(ws_echo PRIVATE
  ws_sys_ctx_posix
)
