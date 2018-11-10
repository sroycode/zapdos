include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_STORE_SOURCES
		StoreBase.cc
		StoreLevel.cc
		StoreTrans.cc
		CacheContainer.cc

		ProfileTable.cc
		SimpleTemplateTable.cc
		LookupRecordTable.cc

		ProfileService.cc
		LookupRecordService.cc
		PhotonService.cc
   )

add_library(zpds_store STATIC ${ZPDS_STORE_SOURCES})

target_link_libraries(zpds_store
	zpds_remote
	zpds_search
	zpds_jamspell
)
