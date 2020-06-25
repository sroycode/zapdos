include_directories(${CMAKE_CURRENT_BINARY_DIR})

# zpds_dumpdb

add_executable(zpds_dumpdb
	DumpDb.cc
	../store/StoreBase.cc
	../store/StoreLevel.cc
)
target_link_libraries(zpds_dumpdb
	${ZPDS_LIB_DEPS}
	zpds_proto
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_dumpdb)

# zpds_spell

add_executable(zpds_spell
	TestSpell.cc
	../jamspell/phf.cc
	../jamspell/city.cc
	../jamspell/JamUtils.cc
	../jamspell/BloomFilter.cc
	../jamspell/LangModel.cc
	../jamspell/PerfectHash.cc
	../jamspell/SpellCorrector.cc
)
target_link_libraries(zpds_spell
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_spell)

# zpds_adddata

add_executable(zpds_adddata
	AddData.cc
	../query/ProtoJson.cc
	../utils/SplitWith.cc
)
target_link_libraries(zpds_adddata
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	zpds_proto
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_adddata)

# zpds_extractwiki

add_executable(zpds_extractwiki
	ExtractWiki.cc
	../utils/SplitWith.cc
)
target_link_libraries(zpds_extractwiki
	${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	zpds_async
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_extractwiki)

# zpds_xapindex

if (ZPDS_BUILD_WITH_XAPIAN)
add_executable(zpds_xapindex
	XapIndex.cc
	../utils/SplitWith.cc
	../search/BaseUtils.cc
	../search/WriteIndex.cc
	../search/KrovetzStemmer.cc
	../search/GeoHashHelper.cc
	../search/DistanceSlabKeyMaker.cc
	../search/IndexBase.cc
	../search/IndexLocal.cc
	../search/IndexWiki.cc
	../store/StoreBase.cc
	../store/StoreLevel.cc
	../store/CacheContainer.cc
)
target_link_libraries(zpds_xapindex
	${ZPDS_LIB_DEPS}
	zpds_proto
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_xapindex)
endif()


# all targets

install(TARGETS ${TOOL_TARGETS}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

