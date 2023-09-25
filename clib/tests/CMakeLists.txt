cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(main main.cpp)


# Include the Google Test headers
target_include_directories(main PRIVATE ${googletest_SOURCE_DIR}/googletest/include)


target_link_libraries(main PRIVATE ${CMAKE_SOURCE_DIR}/../../target/aarch64-apple-darwin/release/libclvm.a)
target_link_libraries(main PRIVATE gtest)
target_link_libraries(main PRIVATE gtest_main)