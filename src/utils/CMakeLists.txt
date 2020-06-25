include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_UTILS_SOURCES
	B64String.cc
	S64String.cc
	CfgFileOptions.cc
	SplitWith.cc
)

add_library(zpds_utils STATIC ${ZPDS_UTILS_SOURCES})
