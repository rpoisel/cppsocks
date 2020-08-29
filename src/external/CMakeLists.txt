add_subdirectory(spdlog)

add_library(cppcodec_header_only INTERFACE)
target_include_directories(cppcodec_header_only INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cppcodec)

add_library(digestpp_header_only INTERFACE)
target_include_directories(digestpp_header_only INTERFACE ${CMAKE_CURRENT_LIST_DIR}/digestpp)

# TODO add CMake config for digestpp
