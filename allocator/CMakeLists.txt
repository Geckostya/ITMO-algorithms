cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(test)
add_subdirectory(googletest)

enable_testing()
include_directories(googletest/include googletest)

if(CMAKE_BUILD_TYPE MATCHES Release)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")
endif()