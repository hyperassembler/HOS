cmake_minimum_required(VERSION 2.8.4)
project(secX)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")


file(GLOB_RECURSE SOURCE_FILES ./*.h ./*.c)
file(GLOB_RECURSE TestFiles ./test/*.c ./lib/*.c)

add_executable(Workspace ${SOURCE_FILES})

#include_directories(test/include)
#add_definitions(-DTDBG)
#add_executable(Test ${TestFiles})