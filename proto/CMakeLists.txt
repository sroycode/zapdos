PROTOBUF_GENERATE_CPP(ZPDS_PB_SRC_ALL ZPDS_PB_HDR_ALL
	Store.proto
	Query.proto
	Hrpc.proto
	Search.proto
)

set(PROTO_SOURCES ${ZPDS_PB_SRC_ALL})
set(ZPDS_SOURCES ${PROTO_SOURCES})
add_library(zpds_proto STATIC ${ZPDS_SOURCES})
