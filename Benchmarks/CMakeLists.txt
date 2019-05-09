add_executable(wallclock wallclock.cpp ../src/base/HeapBase.cpp)
add_executable(console console.cpp ../src/base/HeapBase.cpp)
add_executable(graph graph.cpp graph.hpp ../src/base/HeapBase.cpp)

target_include_directories(wallclock PRIVATE ../src)
target_include_directories(console PRIVATE ../src)
target_include_directories(graph PRIVATE ../src)