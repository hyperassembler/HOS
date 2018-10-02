cmake_minimum_required(VERSION 2.8.4)
project(secX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

file(GLOB_RECURSE hal_src  ./inc/*.h ./hal/*.c ./hal/*.h ./common/*.c)
file(GLOB_RECURSE kernel_src ./inc/*.h ./kernel/*.c ./kernel/*.h ./common/*.c)
file(GLOB_RECURSE test_src  ./inc/*.h ./test/*.c ./test/*.h ./common/*.c)
include_directories(inc)

# HAL
#include_directories(hal/inc)
#add_executable(hal ${hal_src})


# KERNEL
include_directories(kernel/inc)
add_executable(kernel ${kernel_src})

# KERNEL + TESTS
#include_directories(test/inc)
#include_directories(kernel/inc)
#add_definitions(-DTDBG)
#add_executable(tests ${test_src} ${kernel_src})
