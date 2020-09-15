cmake_minimum_required(VERSION 3.16)
project(design_patterns)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fconcepts -fconcepts-diagnostics-depth=10")


add_executable(design_patterns main.cpp behavioral/iterator.h)