add_executable(ws_echo
  main.cpp
  system_posix.cpp
)

target_include_directories(ws_echo PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(ws_echo PRIVATE
  ws
  wslay
)
