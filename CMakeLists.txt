cmake_minimum_required(VERSION 3.1)

project(ser)

set(PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}/build CACHE PATH "")
set(CMAKE_CXX_FLAGS "-Wall -g -std=c++11")

# Headers
include_directories(archiver/include)

# Projects
link_directories(archiver)
add_subdirectory(psevtool)
