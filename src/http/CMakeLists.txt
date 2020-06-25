include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_HTTP_SOURCES
	WebServer.cc
)

add_library(zpds_http STATIC ${ZPDS_HTTP_SOURCES})
