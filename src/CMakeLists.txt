find_package(Threads)

add_library(ws STATIC
  ws.cpp
)

target_include_directories(ws PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(ws PUBLIC
  Threads::Threads
 )

target_compile_options(ws PUBLIC
  -Wall
  -Wextra
 )

add_subdirectory(examples)