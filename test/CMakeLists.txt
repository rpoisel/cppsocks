add_subdirectory(external)

add_library(TestMain STATIC
  test_main.cpp
)

target_link_libraries(TestMain PUBLIC Catch2::Catch2)

### Simple
add_executable(test_simple_app
  test_simple.cpp
)

target_link_libraries(test_simple_app PRIVATE
  TestMain
)

add_test(NAME test_simple COMMAND test_simple_app)

