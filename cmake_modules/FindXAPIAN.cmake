
# - Try to find XAPIAN
# Once done this will define
#  XAPIAN_FOUND - System has XAPIAN
#  XAPIAN_INCLUDE_DIRS - The XAPIAN include directories
#  XAPIAN_LIBRARIES - The libraries needed to use XAPIAN
#  XAPIAN_DEFINITIONS - Compiler switches required for using XAPIAN

find_package(PkgConfig)
pkg_check_modules(PC_XAPIAN QUIET libsodium)
set(XAPIAN_DEFINITIONS ${PC_XAPIAN_CFLAGS_OTHER})

find_path(XAPIAN_INCLUDE_DIR xapian.h
       HINTS
       ${PC_XAPIAN_INCLUDEDIR} ${PC_XAPIAN_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/thirdparty/include
       /opt/local/include /usr/local/include )

find_library(XAPIAN_LIBRARY NAMES xapian
       HINTS ${PC_XAPIAN_LIBDIR} ${PC_XAPIAN_LIBRARY_DIRS}
             ${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
       /opt/local/lib /usr/local/lib64 /usr/local/lib)

set(XAPIAN_LIBRARIES ${XAPIAN_LIBRARY} )
set(XAPIAN_INCLUDE_DIRS ${XAPIAN_INCLUDE_DIR} )

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XAPIAN_FOUND to TRUE
# if all listed variables are TRUE

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(XAPIAN_PROCESS_INCLUDES XAPIAN_INCLUDE_DIR XAPIAN_INCLUDE_DIRS)
set(XAPIAN_PROCESS_LIBS XAPIAN_LIBRARY XAPIAN_LIBRARIES)
find_package_handle_standard_args(XAPIAN REQUIRED_VARS XAPIAN_INCLUDE_DIR XAPIAN_LIBRARY)
mark_as_advanced(XAPIAN_INCLUDE_DIR XAPIAN_LIBRARY )
