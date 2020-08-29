add_executable(ws_push
  main.cpp
  handler.cpp
  pusher.cpp
)

target_include_directories(ws_push PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(ws_push PRIVATE
  socks_http_header_only
  sys_ctx_posix_header_only
)

