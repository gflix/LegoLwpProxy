cmake_minimum_required(VERSION 3.16)
project(LegoLwpProxy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(sdbus-c++ REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src)
