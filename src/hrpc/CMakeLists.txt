include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_HRPC_SOURCES
	HrpcClient.cc
	RemoteKeeper.cc
	SyncServer.cc
)

add_library(zpds_hrpc STATIC ${ZPDS_HRPC_SOURCES})
