add_subdirectory(async++)
add_subdirectory(cpr)

add_subdirectory(remote)
add_subdirectory(search)
add_subdirectory(jamspell)
add_subdirectory(utils)
add_subdirectory(store)
add_subdirectory(hrpc)

add_subdirectory(query)
add_subdirectory(http)
add_subdirectory(main)

if (ZPDS_BUILD_TOOLS)
	add_subdirectory(tools)
endif()
