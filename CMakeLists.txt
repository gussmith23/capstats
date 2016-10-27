cmake_minimum_required(VERSION 3.6)

project(capstats)
file(GLOB CAPSTATS_SOURCES "src/*.cpp")
add_executable(capstats_server "${CAPSTATS_SOURCES}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include_directories("${CMAKE_SOURCE_DIR}/src")

# restbed
find_package(restbed REQUIRED)
include_directories(${restbed_INCLUDE})
target_link_libraries(capstats_server ${restbed_LIBRARY})

# otl
include_directories("${CMAKE_SOURCE_DIR}/dependency/otl")

# catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "${CMAKE_SOURCE_DIR}/dependency/catch")

# odbc
find_package(odbc REQUIRED)
target_link_libraries(capstats_server ${ODBC_LIBRARIES})