cmake_minimum_required(VERSION 3.9)
project(bullshits)

set(CMAKE_CXX_STANDARD 17)

add_executable(queryable tests.cpp queryable.hpp)
add_executable(q2 main.cpp queryable2.hpp)