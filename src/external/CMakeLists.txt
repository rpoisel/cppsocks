add_subdirectory(spdlog)

add_library(cppcodec INTERFACE)
target_include_directories(cppcodec INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cppcodec)

add_library(digestpp INTERFACE)
target_include_directories(digestpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/digestpp)

# TODO add CMake config for digestpp
