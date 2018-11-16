include_directories(${CMAKE_CURRENT_BINARY_DIR})

# zpds_dumpdb

add_executable(zpds_dumpdb
	DumpDb.cc
)
target_link_libraries(zpds_dumpdb
	${ZPDS_LIB_DEPS}
	zpds_store
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_dumpdb)


# zpds_addlocal

add_executable(zpds_addlocal
	AddLocalData.cc
	LineReader.cc
	../query/ProtoJson.cc
)
target_link_libraries(zpds_addlocal
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${CURL_LIBRARIES}
	zpds_proto
	zpds_cpr
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_addlocal)

# zpds_addtext

add_executable(zpds_addtext
	AddTextData.cc
	LineReader.cc
	../query/ProtoJson.cc
)
target_link_libraries(zpds_addtext
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${CURL_LIBRARIES}
	zpds_proto
	zpds_cpr
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_addtext)

# zpds_spell

add_executable(zpds_spell
	TestSpell.cc
	../jamspell/phf.cc
	../jamspell/JamUtils.cc
	../jamspell/BloomFilter.cc
	../jamspell/LangModel.cc
	../jamspell/PerfectHash.cc
	../jamspell/SpellCorrector.cc
)
target_link_libraries(zpds_spell
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${CITYHASH_LIBRARIES}
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_spell)

# zpds_nomina

add_executable(zpds_nomina
	NominaCsv.cc
)

find_package(PQXX REQUIRED)
include_directories(${PQXX_INCLUDE_DIRS})

target_link_libraries(zpds_nomina
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${PQXX_LIBRARIES}
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	zpds_cpr
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_nomina)

# all targets

install(TARGETS ${TOOL_TARGETS}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

