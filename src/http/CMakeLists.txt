add_library(socks_http STATIC
  socks_http.cpp
  socks_http_handler.cpp
)

target_include_directories(socks_http PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(socks_http PRIVATE
  socks_tcp
)