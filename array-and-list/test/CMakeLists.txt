cmake_minimum_required(VERSION 3.13)

add_executable(tests arrayTests.cpp listTests.cpp)

include_directories(../include)
target_link_libraries(tests gtest gtest_main)
add_test(tests tests)