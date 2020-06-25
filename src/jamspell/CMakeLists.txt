include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_JAMSPELL_SOURCES
	city.cc
	phf.cc
	JamUtils.cc
	BloomFilter.cc
	LangModel.cc
	PerfectHash.cc
	SpellCorrector.cc

	StoreJam.cc
)

add_library(zpds_jamspell STATIC ${ZPDS_JAMSPELL_SOURCES})
