include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_REMOTE_SOURCES
	KrovetzStemmer.cc
	GeoHashHelper.cc
)

add_library(zpds_remote STATIC ${ZPDS_REMOTE_SOURCES})

target_link_libraries(zpds_remote
	zpds_proto
)
