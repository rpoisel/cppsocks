cmake_minimum_required(VERSION 3.9)
project(cppsocks)

include(cmake/StaticAnalyzers.cmake)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)
