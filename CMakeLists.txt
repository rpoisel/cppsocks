cmake_minimum_required(VERSION 3.9)
project(cppsocks)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/StaticAnalyzers.cmake)
add_compile_options(-Wall -Wextra -Werror -pedantic -pedantic-errors)
if (COVERAGE)
  add_compile_options(--coverage -O0)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${SHARED_EXE_LINKER_FLAGS} --coverage")
endif ()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)
