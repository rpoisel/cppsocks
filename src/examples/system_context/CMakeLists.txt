add_library(sys_ctx_posix STATIC
  system_posix.cpp
)

target_include_directories(sys_ctx_posix PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sys_ctx_posix PUBLIC
  socks_tcp
)
