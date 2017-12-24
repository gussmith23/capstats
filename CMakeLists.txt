cmake_minimum_required(VERSION 3.6)

project(capstats)

# make sure we get the right C++ version
# TODO(gus) the first line shouldn't be needed, but is. fix it
set(CMAKE_CXX_FLAGS "-std=c++11")
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CMAKE_CXX_EXTENSIONS OFF)

set(SERVER_SOURCES
      "${CMAKE_SOURCE_DIR}/src/api_key_dao.cpp"
      "${CMAKE_SOURCE_DIR}/src/game.cpp"
      "${CMAKE_SOURCE_DIR}/src/game_dao.cpp"
      "${CMAKE_SOURCE_DIR}/src/player.cpp"
      "${CMAKE_SOURCE_DIR}/src/player_dao.cpp"
      "${CMAKE_SOURCE_DIR}/src/points_dao.cpp"
      "${CMAKE_SOURCE_DIR}/src/team_dao.cpp"
      # TODO(gus): I don't think this should go here.
      "${CMAKE_SOURCE_DIR}/src/capstats_config.cpp"
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

add_executable(capstats_server "${CMAKE_SOURCE_DIR}/src/main.cpp" "${CMAKE_SOURCE_DIR}/src/capstats_server.cpp")
target_link_libraries(capstats_server capstats)
target_link_libraries(capstats_server ${restbed_LIBRARY})
add_dependencies(capstats_server restbed)
target_link_libraries(capstats_server ${ODBC_LIBRARIES})
target_link_libraries(capstats_server ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_server Threads::Threads)

set(CAPSTATS_TEST_SOURCES
      "${CMAKE_SOURCE_DIR}/test/capstats_tests.cpp"
      "${CMAKE_SOURCE_DIR}/test/capstats_config_test.cpp")
add_executable(capstats_tests ${CAPSTATS_TEST_SOURCES})
target_link_libraries(capstats_tests capstats)
add_dependencies(capstats_tests restbed)
target_link_libraries(capstats_tests Catch)
target_link_libraries(capstats_tests ${restbed_LIBRARY})
target_link_libraries(capstats_tests ${ODBC_LIBRARIES})
target_link_libraries(capstats_tests ${JSONBOX_LIBRARIES})
target_link_libraries(capstats_tests Threads::Threads)

enable_testing()
add_test(NAME capstats_tests COMMAND capstats_tests)
add_custom_target(copy-test-config-1 DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test_config_1.cfg")
add_dependencies(capstats_tests copy-test-config-1)
add_test(NAME server_tests COMMAND python -m unittest server_tests)

# add rules for each test resource. these rules will copy the test resource to the build dir when needed.
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

  # list of files for which we add a copy rule
  set(data_SHADOW "test_config_1.cfg")

  foreach(item IN LISTS data_SHADOW)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/test/resources/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/resources/${item}"
        )
  endforeach()
endif()
