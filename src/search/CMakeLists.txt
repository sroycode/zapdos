include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_SEARCH_SOURCES
	BaseUtils.cc
	WriteIndex.cc
	ReadIndex.cc
	KrovetzStemmer.cc
	GeoHashHelper.cc
	DistanceSlabKeyMaker.cc

	IndexBase.cc
	IndexLocal.cc
	IndexWiki.cc

	SearchBase.cc
	SearchCache.cc
	SearchLocal.cc
	SearchWiki.cc
)

add_library(zpds_search STATIC ${ZPDS_SEARCH_SOURCES})
