include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_QUERY_SOURCES
	ProtoJson.cc
	ProtoForm.cc
	HttpClient.cc
)

add_library(zpds_query STATIC ${ZPDS_QUERY_SOURCES})
target_link_libraries(zpds_query
	zpds_hrpc
	${PROTOBUF_LIBRARIES}
)
