add_executable(ws_push
  main.cpp
)

target_link_libraries(ws_push PRIVATE
  ws_sys_ctx_posix
)
