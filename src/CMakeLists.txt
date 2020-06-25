add_subdirectory(async++)

add_subdirectory(crypto)
add_subdirectory(utils)

if (ZPDS_BUILD_WITH_XAPIAN)
	add_subdirectory(jamspell)
	add_subdirectory(search)
endif()

add_subdirectory(store)
add_subdirectory(hrpc)

add_subdirectory(query)
add_subdirectory(http)
add_subdirectory(main)


if (ZPDS_BUILD_TOOLS)
	add_subdirectory(tools)
endif()
