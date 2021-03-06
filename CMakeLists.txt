cmake_minimum_required(VERSION 3.1)

project(priority-queues)

# Single header libraries
set(SINGLE_HEADER_LIBS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/include")

# Configure compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  # using regular Clang or AppleClang or GCC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic -Wold-style-cast -I Benchmarks/bricks -O4")
  # the following two lines switch coverage analysis on
  # uncomment them if you want to use coverage analysis
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_library(base STATIC src/base/HeapBase.cpp)

add_subdirectory(src/ExplicitHeap)
add_subdirectory(src/FibonacciHeap)
add_subdirectory(src/ImplicitHeap)
add_subdirectory(src/BinomialHeap)
add_subdirectory(src/ViolationHeap)
add_subdirectory(src/RankPairingHeap)
add_subdirectory(src/helpers/queryable)
add_subdirectory(Benchmarks)
