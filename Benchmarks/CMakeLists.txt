set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic -Wold-style-cast -I Benchmarks/bricks -O4")

add_executable(wallclock wallclock.cpp ../src/base/HeapBase.cpp benchmark.hpp)
add_executable(console console.cpp ../src/base/HeapBase.cpp)
add_executable(graph graph.cpp graph.hpp ../src/base/HeapBase.cpp)

target_include_directories(wallclock PRIVATE ../src)
target_include_directories(console PRIVATE ../src)
target_include_directories(graph PRIVATE ../src)