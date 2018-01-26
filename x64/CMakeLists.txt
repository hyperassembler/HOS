cmake_minimum_required(VERSION 2.8.4)
project(Workspace)

include_directories(src/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SOURCE_FILES ./*.h ./*.c)

add_executable(Workspace ${SOURCE_FILES})