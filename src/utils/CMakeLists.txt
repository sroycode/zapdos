include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_UTILS_SOURCES
	CfgFileOptions.cc
)

add_library(zpds_utils STATIC ${ZPDS_UTILS_SOURCES})
