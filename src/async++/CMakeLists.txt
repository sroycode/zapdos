include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_ASYNCPP_SOURCES
	scheduler.cpp
	threadpool_scheduler.cpp
)

add_library(zpds_async STATIC ${ZPDS_ASYNCPP_SOURCES})
