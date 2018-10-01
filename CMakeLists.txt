cmake_minimum_required(VERSION 2.8.4)
project(secX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

file(GLOB_RECURSE hal_src ./hal/*.h ./hal/*.c ./common/*.h ./common/*.c)
file(GLOB_RECURSE kernel_src ./kernel/*.h ./kernel/*.c ./common/*.h ./common/*.c)
file(GLOB_RECURSE test_src ./test/*.h ./test/*.c ./common/*.h ./common/*.c)
include_directories(inc)

#KERNEL + HAL
include_directories(kernel/inc)
include_directories(hal/inc)
add_executable(kernel ${kernel_src} ${hal_src})

# KERNEL + TESTS
#include_directories(test/inc)
#include_directories(kernel/inc)
#add_definitions(-DTDBG)
#add_executable(tests ${test_src} ${kernel_src})
