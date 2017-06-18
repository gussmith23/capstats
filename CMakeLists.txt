cmake_minimum_required(VERSION 3.6)

project(capstats)
add_executable(capstats_server 
		"src/game.cpp"
		"src/game_dao.cpp"
		"src/player.cpp"
		"src/player_dao.cpp"
		"src/points_dao.cpp"
		"src/team_dao.cpp"
		"src/capstats_server.cpp"
		"src/main.cpp")

include_directories("${CMAKE_SOURCE_DIR}/src")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

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

# JsonBox
find_package(JsonBox REQUIRED)
include_directories(${JSONBOX_INCLUDE_DIR})
target_link_libraries(capstats_server ${JSONBOX_LIBRARIES})

# Native threads
find_package(Threads REQUIRED)
target_link_libraries(capstats_server Threads::Threads)

# tests
file(GLOB CAPSTATS_TEST_SOURCES "test/*.cpp" "src/*")
list(REMOVE_ITEM CAPSTATS_TEST_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
add_executable(capstats_tests ${CAPSTATS_TEST_SOURCES})
target_link_libraries(capstats_tests Catch)
target_link_libraries(capstats_tests ${restbed_LIBRARY})
target_link_libraries(capstats_tests ${ODBC_LIBRARIES})
target_link_libraries(capstats_tests ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_tests Threads::Threads)

enable_testing()
add_test(NAME capstats_tests COMMAND capstats_tests)
add_test(NAME server_tests WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test" COMMAND python -m unittest server_tests)
