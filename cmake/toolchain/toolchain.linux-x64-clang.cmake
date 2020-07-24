if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_CXX_COMPILER clang-10)
  set(CMAKE_C_COMPILER clang-10)
else()
  message(FATAL_ERROR Unsupported build platform.)
endif()

