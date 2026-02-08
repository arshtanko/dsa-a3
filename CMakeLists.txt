cmake_minimum_required(VERSION 4.1)
project(a3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(a3
        RandomStreamGen.cpp
        RandomStreamGen.h)
