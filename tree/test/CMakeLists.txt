cmake_minimum_required(VERSION 3.13)

add_executable(tests gtest.cpp)

include_directories(../includes)
target_link_libraries(tests gtest gtest_main)
add_test(tests tests)