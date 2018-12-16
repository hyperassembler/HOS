cmake_minimum_required(VERSION 2.8.4)
project(secX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

file(GLOB_RECURSE kernel_src ./inc/*.h ke/*.c ke/*.h ./arch/*.c ./arch/*.h)
include_directories(inc)

# KERNEL
add_executable(kernel ${kernel_src})

# KERNEL + TESTS
#include_directories(test/inc)
#include_directories(kernel/inc)
#add_definitions(-DTDBG)
#add_executable(tests ${test_src} ${kernel_src})

