include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_QUERY_SOURCES
	ProtoJson.cc
	ProtoForm.cc
)

add_library(zpds_query STATIC ${ZPDS_QUERY_SOURCES})
