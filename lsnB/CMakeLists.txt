cmake_minimum_required(VERSION 3.17)

project(shm)

set(EXECUTABLE_OUTPUT_PATH ../)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")


add_executable(shm shm.c)