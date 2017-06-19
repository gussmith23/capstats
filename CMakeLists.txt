cmake_minimum_required(VERSION 3.6)

project(capstats)

set(SERVER_SOURCES
      "src/game.cpp"
      "src/game_dao.cpp"
      "src/player.cpp"
      "src/player_dao.cpp"
      "src/points_dao.cpp"
      "src/team_dao.cpp"
      "src/capstats_server.cpp"
      "src/game.h"
      "src/game_dao.h"
      "src/player.h"
      "src/player_dao.h"
      "src/points_dao.h"
      "src/team_dao.h"
      "src/capstats_server.h")
set(TEST_SOURCES
      "test/capstats_tests.cpp")
      
add_executable(capstats_server ${SERVER_SOURCES} "src/main.cpp")

# make sure we get the right C++ version
# TODO(gus) the first line shouldn't be needed, but is. fix it
set(CMAKE_CXX_FLAGS "-std=c++11")
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CMAKE_CXX_EXTENSIONS OFF)

include_directories("${CMAKE_SOURCE_DIR}/src")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# restbed
find_package(restbed REQUIRED)
include_directories(${restbed_INCLUDE})
target_link_libraries(capstats_server ${restbed_LIBRARY})
add_dependencies(capstats_server restbed)
#add_library(restbed_lib STATIC IMPORTED)
# find_package adds external restbed project - now we make sure it builds
#add_dependencies(restbed_lib restbed) 
#target_link_libraries

# otl
include_directories("${CMAKE_SOURCE_DIR}/dependency/otl")

# catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "${CMAKE_SOURCE_DIR}/dependency/catch")

# odbc
find_package(odbc REQUIRED)
target_link_libraries(capstats_server ${ODBC_LIBRARIES})

# JsonBox
find_package(JsonBox REQUIRED)
include_directories(${JSONBOX_INCLUDE_DIR})
target_link_libraries(capstats_server ${JSONBOX_LIBRARIES})

# Native threads
find_package(Threads REQUIRED)
target_link_libraries(capstats_server Threads::Threads)

# tests
add_executable(capstats_tests ${SERVER_SOURCES} ${TEST_SOURCES})
add_dependencies(capstats_tests restbed)
target_link_libraries(capstats_tests Catch)
target_link_libraries(capstats_tests ${restbed_LIBRARY})
target_link_libraries(capstats_tests ${ODBC_LIBRARIES})
target_link_libraries(capstats_tests ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_tests Threads::Threads)

enable_testing()
add_test(NAME capstats_tests COMMAND capstats_tests)
add_test(NAME server_tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test" COMMAND python -m unittest server_tests)
