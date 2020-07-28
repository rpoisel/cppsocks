add_library(ws_sys_ctx_posix STATIC
  system_posix.cpp
)

target_include_directories(ws_sys_ctx_posix PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(ws_sys_ctx_posix PUBLIC
  ws
)

add_executable(ws_echo
  main.cpp
)

target_link_libraries(ws_echo PRIVATE
  ws_sys_ctx_posix
)
