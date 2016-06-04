cmake_minimum_required(VERSION 2.8.4)
project(Workspace)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list ./*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
       SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(header_dirs)

include_directories(${header_dirs})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SOURCE_FILES ./*.h ./*.c)

add_executable(Workspace ${SOURCE_FILES})