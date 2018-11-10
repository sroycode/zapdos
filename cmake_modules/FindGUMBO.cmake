# - Try to find GUMBO
# Once done this will define
#  GUMBO_FOUND - System has GUMBO
#  GUMBO_INCLUDE_DIRS - The GUMBO include directories
#  GUMBO_LIBRARIES - The libraries needed to use GUMBO

find_package(PkgConfig)
pkg_check_modules(PC_GUMBO QUIET gumbo)
set(GUMBO_DEFINITIONS ${PC_GUMBO_CFLAGS_OTHER})

find_path(GUMBO_INCLUDE_DIR gumbo.h
	HINTS ${PC_GUMBO_INCLUDEDIR} ${PC_GUMBO_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/include
	/usr/include /usr/local/include /opt/local/include
)
find_library(GUMBO_LIBRARY NAMES libgumbo.a
	HINTS ${PC_GUMBO_LIBDIR} ${PC_GUMBO_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

set(GUMBO_LIBRARIES ${GUMBO_LIBRARY} )
set(GUMBO_INCLUDE_DIRS ${GUMBO_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(GUMBO DEFAULT_MSG GUMBO_INCLUDE_DIR GUMBO_LIBRARY)
else ()
  find_package_handle_standard_args(GUMBO REQUIRED_VARS GUMBO_INCLUDE_DIR GUMBO_LIBRARY)
endif ()

if (GUMBO_FOUND)
set(GUMBO_PROCESS_INCLUDES GUMBO_INCLUDE_DIR GUMBO_INCLUDE_DIRS)
set(GUMBO_PROCESS_LIBS GUMBO_LIBRARY GUMBO_LIBRARIES)
endif()
