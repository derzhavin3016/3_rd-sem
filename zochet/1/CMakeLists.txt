cmake_minimum_required(VERSION 3.13)

project(fst)

set(EXECUTABLE_OUTPUT_PATH ../)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(1.o 1.c)