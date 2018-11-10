
# - Try to find CITYHASH
# Once done this will define
#  CITYHASH_FOUND - System has CITYHASH
#  CITYHASH_INCLUDE_DIRS - The CITYHASH include directories
#  CITYHASH_LIBRARIES - The libraries needed to use CITYHASH
#  CITYHASH_DEFINITIONS - Compiler switches required for using CITYHASH

find_package(PkgConfig)
pkg_check_modules(PC_CITYHASH QUIET libsodium)
set(CITYHASH_DEFINITIONS ${PC_CITYHASH_CFLAGS_OTHER})

find_path(CITYHASH_INCLUDE_DIR city.h
       HINTS
       ${PC_CITYHASH_INCLUDEDIR} ${PC_CITYHASH_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /opt/local/include /usr/local/include )

find_library(CITYHASH_LIBRARY NAMES cityhash
       HINTS ${PC_CITYHASH_LIBDIR} ${PC_CITYHASH_LIBRARY_DIRS}
             ${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(CITYHASH_LIBRARIES ${CITYHASH_LIBRARY} )
set(CITYHASH_INCLUDE_DIRS ${CITYHASH_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CITYHASH_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(CITYHASH_PROCESS_INCLUDES CITYHASH_INCLUDE_DIR CITYHASH_INCLUDE_DIRS)
set(CITYHASH_PROCESS_LIBS CITYHASH_LIBRARY CITYHASH_LIBRARIES)
find_package_handle_standard_args(CITYHASH REQUIRED_VARS CITYHASH_INCLUDE_DIR CITYHASH_LIBRARY)
#mark_as_advanced(CITYHASH_INCLUDE_DIR CITYHASH_LIBRARY )
