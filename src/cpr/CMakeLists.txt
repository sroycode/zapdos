include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_CPR_SOURCES
	auth.cpp
	cookies.cpp
	cprtypes.cpp
	digest.cpp
	error.cpp
	multipart.cpp
	parameters.cpp
	payload.cpp
	proxies.cpp
	session.cpp
	timeout.cpp
	util.cpp
	ssl_options.cpp
)

add_library(zpds_cpr STATIC ${ZPDS_CPR_SOURCES})
target_link_libraries(zpds_cpr
	${CURL_LIBRARIES}
)
