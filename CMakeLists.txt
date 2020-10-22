cmake_minimum_required(VERSION 3.16)
include(cmake/CPM.cmake)
project(design_patterns)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fconcepts -fcoroutines -fconcepts-diagnostics-depth=10")

include_directories(include)



add_executable(design_patterns main.cpp behavioral/iterator.h)
target_link_libraries(design_patterns)