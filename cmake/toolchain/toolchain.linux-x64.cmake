if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_CXX_COMPILER g++)
  set(CMAKE_C_COMPILER gcc)
else()
  message(FATAL_ERROR Unsupported build platform.)
endif()

