include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_WORK_SOURCES
	WorkServer.cc
)

add_library(zpds_work STATIC ${ZPDS_WORK_SOURCES})
target_link_libraries(zpds_work
	zpds_hrpc
)

set(ZPDS_LIB_DEPS
	${ZPDS_LIB_DEPS}
	zpds_utils
	zpds_async
	zpds_work
	zpds_http
)

set(ZPDS_SOURCES
	DefaultServer.cc
)

add_executable(zpds_server ${ZPDS_SOURCES})
target_link_libraries(zpds_server ${ZPDS_LIB_DEPS})

install(TARGETS ${ZPDS_TARGET_LIBS} zpds_server
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

