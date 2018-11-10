include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_SEARCH_SOURCES
		TrieBase.cc
		StoreTrie.cc
		SearchTrie.cc
   )

add_library(zpds_search STATIC ${ZPDS_SEARCH_SOURCES})

target_link_libraries(zpds_search
	zpds_remote
	${XAPIAN_LIBRARIES}
)
