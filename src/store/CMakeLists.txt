include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_STORE_SOURCES
	StoreBase.cc
	StoreLevel.cc
	StoreTrans.cc
	CacheContainer.cc
	TempNameCache.cc

	TagDataTable.cc
	ExterDataTable.cc
	UserDataTable.cc
	CategoryTable.cc

	LocalDataTable.cc
	WikiDataTable.cc

	HandleSession.cc
	HandleTagData.cc
	HandleGeneric.cc
	HandleStringData.cc

	CredServiceBase.cc
	UserCredService.cc
	ExterCredService.cc

	TagDataService.cc
	CategoryService.cc

	UserDataService.cc
	ExterDataService.cc

	ItemDataService.cc
	LocalDataService.cc
	WikiDataService.cc

)

add_library(zpds_store STATIC ${ZPDS_STORE_SOURCES})
