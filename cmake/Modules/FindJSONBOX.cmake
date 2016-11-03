#
# Try to find JsonBox library and include path.
# Once done this will define
#
# JSONBOX_FOUND
# JSONBOX_INCLUDE_DIR
# JSONBOX_LIBRARIES
# JSONBOX_ROOT
#

set(FIND_JSONBOX_PATHS
    ${JSONBOX_ROOT}
    $ENV{JSONBOX_ROOT}
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    "/Program Files (x86)"
    "/Program Files")

if(JSONBOX_ROOT)
	SET(JSONBOX_INCLUDE_DIR "${JSONBOX_ROOT}/include")
	SET(JSONBOX_LIBRARIES "${JSONBOX_ROOT}/lib/libJsonBox.a")
	SET(JSONBOX_FOUND 1)
endif(JSONBOX_ROOT)

find_path(JSONBOX_INCLUDE_DIR JsonBox/include/JsonBox.h
          PATH_SUFFIXES include
          PATHS ${FIND_JSONBOX_PATHS})

find_library(JSONBOX_LIBRARY
		NAMES JsonBox 
		PATH_SUFFIXES lib
		PATHS ${FIND_JSONBOX_PATHS})
    
find_library(JSONBOX_LIBRARY_DEBUG
		NAMES JsonBox_d
		PATH_SUFFIXES lib
		PATHS ${FIND_JSONBOX_PATHS})
  
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSONBOX DEFAULT_MSG JSONBOX_LIBRARY JSONBOX_INCLUDE_DIR)

set(JSONBOX_INCLUDE_DIR "${JSONBOX_INCLUDE_DIR}/JsonBox/include")
set(JSONBOX_LIBRARIES optimized "${JSONBOX_LIBRARY}" debug "${JSONBOX_LIBRARY_DEBUG}")

if(NOT JSONBOX_FOUND)
	set(FIND_JSONBOX_ERROR "Could NOT find JSONBOX")
	if(JSONBOX_FIND_REQUIRED)
		message(FATAL_ERROR ${FIND_JSONBOX_ERROR})
	endif()
endif()
