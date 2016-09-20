cmake_minimum_required(VERSION 3.6)

project(capstats)
add_executable(capstats_server capstats_server.cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# restbed
find_package(restbed REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/dependency/otl")
target_link_libraries(capstats_server ${restbed_LIBRARY})

# otl
include_directories(${restbed_INCLUDE})
