set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ibricks")

add_executable(wallclock wallclock.cpp)

target_include_directories(wallclock PRIVATE ../src)
target_include_directories(wallclock PRIVATE bricks)