cmake_minimum_required(VERSION 3.6)

project(capstats)

# make sure we get the right C++ version
# TODO(gus) the first line shouldn't be needed, but is. fix it
set(CMAKE_CXX_FLAGS "-std=c++11")
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CMAKE_CXX_EXTENSIONS OFF)

set(SERVER_SOURCES
      "src/api_key_dao.cpp"
      "src/game.cpp"
      "src/game_dao.cpp"
      "src/player.cpp"
      "src/player_dao.cpp"
      "src/points_dao.cpp"
      "src/team_dao.cpp"
      # TODO(gus): I don't think this should go here.
      "src/capstats_config.cpp"
      )

#############EXTERNAL DEPENDENCIES#############################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# restbed
find_package(restbed REQUIRED)
include_directories(${restbed_INCLUDE})

# otl
include_directories("${CMAKE_SOURCE_DIR}/dependency/otl")

# catch
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "${CMAKE_SOURCE_DIR}/dependency/catch")

# odbc
find_package(odbc REQUIRED)

# JsonBox
find_package(JsonBox REQUIRED)
include_directories(${JSONBOX_INCLUDE_DIR})

# Native threads
find_package(Threads REQUIRED)
#############END EXTERNAL DEPENDENCIES##########################################

include_directories("${CMAKE_SOURCE_DIR}/src")

add_library(capstats ${SERVER_SOURCES})

add_executable(capstats_server "src/main.cpp" "src/capstats_server.cpp")
target_link_libraries(capstats_server capstats)
target_link_libraries(capstats_server ${restbed_LIBRARY})
add_dependencies(capstats_server restbed)
target_link_libraries(capstats_server ${ODBC_LIBRARIES})
target_link_libraries(capstats_server ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_server Threads::Threads)

add_executable(capstats_tests "test/capstats_tests.cpp")
target_link_libraries(capstats_tests capstats)
add_dependencies(capstats_tests restbed)
target_link_libraries(capstats_tests Catch)
target_link_libraries(capstats_tests ${restbed_LIBRARY})
target_link_libraries(capstats_tests ${ODBC_LIBRARIES})
target_link_libraries(capstats_tests ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_tests Threads::Threads)

enable_testing()
add_test(NAME capstats_tests COMMAND capstats_tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME server_tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test" COMMAND python -m unittest server_tests)
