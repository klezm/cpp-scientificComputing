cmake_minimum_required(VERSION 3.6)
project(p2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-larmadillo")

set(SOURCE_FILES main.cpp)
add_executable(p2 ${SOURCE_FILES})