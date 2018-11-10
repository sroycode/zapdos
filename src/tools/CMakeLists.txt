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

# zpds_addcsv

add_executable(zpds_addcsv
	AddCsvData.cc
	LineReader.cc
	../query/ProtoJson.cc
)
target_link_libraries(zpds_addcsv
	${CMAKE_THREAD_LIBS_INIT}
	${GLOG_LIBRARIES}
	${GFLAGS_LIBRARIES}
	${Boost_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${CURL_LIBRARIES}
	zpds_proto
	zpds_cpr
)

set(TOOL_TARGETS ${TOOL_TARGETS} zpds_addcsv)

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

