# - Try to find MYSQL
# Once done this will define
#  MYSQL_FOUND - System has MYSQL
#  MYSQL_INCLUDE_DIRS - The MYSQL include directories
#  MYSQL_LIBRARIES - The libraries needed to use MYSQL client

find_package(PkgConfig)
pkg_check_modules(PC_MYSQL QUIET mysqlclient)
set(MYSQL_DEFINITIONS ${PC_MYSQL_CFLAGS_OTHER})

find_path(MYSQL_INCLUDE_DIR mysql/mysql.h
	HINTS ${PC_MYSQL_INCLUDEDIR} ${PC_MYSQL_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/include
	/usr/include /usr/local/include /opt/local/include
)
find_library(MYSQL_LIBRARY NAMES mysqlclient  libmysqlclient
	HINTS ${PC_MYSQL_LIBDIR} ${PC_MYSQL_LIBRARY_DIRS}
	${PROJECT_SOURCE_DIR}/thirdparty/lib64 ${PROJECT_SOURCE_DIR}/thirdparty/lib
	/usr/lib /usr/lib64 /usr/local/lib /opt/local/lib
)

set(MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR} )

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
if (CMAKE_VERSION LESS 2.8.3)
  find_package_handle_standard_args(MYSQL DEFAULT_MSG MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
else ()
  find_package_handle_standard_args(MYSQL REQUIRED_VARS MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
endif ()

if (MYSQL_FOUND)
set(MYSQL_PROCESS_INCLUDES MYSQL_INCLUDE_DIR MYSQL_INCLUDE_DIRS)
set(MYSQL_PROCESS_LIBS MYSQL_LIBRARY MYSQL_LIBRARIES)
endif()
