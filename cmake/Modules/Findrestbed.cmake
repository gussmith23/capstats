# Corvusoft has their own Findrestbed.cmake. However, they assume that you 
# include restbed source with your source. I find this much more reasonable.
# Just install restbed on your system where you normally would. Update the 
# paths as needed.

find_library( restbed_LIBRARY NAMES restbed PATH_SUFFIXES lib PATHS "/usr/local" )
find_path( restbed_INCLUDE restbed PATH_SUFFIXES include )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSONBOX DEFAULT_MSG JSONBOX_LIBRARY JSONBOX_INCLUDE_DIR)

