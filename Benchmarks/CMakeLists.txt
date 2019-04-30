add_executable(wallclock wallclock.cpp ../src/base/HeapBase.cpp)

target_include_directories(wallclock PRIVATE ../src)