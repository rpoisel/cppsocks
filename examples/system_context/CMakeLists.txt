add_library(sys_ctx_posix STATIC
  system_posix.cpp
)

target_include_directories(sys_ctx_posix PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sys_ctx_posix PUBLIC
  socks_tcp
)

add_library(sys_ctx_posix_header_only INTERFACE)

target_compile_definitions(sys_ctx_posix_header_only INTERFACE
  -DSOCKS_HEADER_ONLY=1
)

target_include_directories(sys_ctx_posix_header_only INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sys_ctx_posix_header_only INTERFACE
  socks_tcp_header_only
)
