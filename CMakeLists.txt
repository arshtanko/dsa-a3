cmake_minimum_required(VERSION 4.1)
project(a3)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(a3
        
        RandomStreamGen.cpp
        RandomStreamGen.h
        HashFuncGen.cpp
        HashFuncGen.h)
