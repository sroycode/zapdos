include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ZPDS_CRYPTO_SOURCES
	CryptoBase.cc
	AES128CBC.cc
)

add_library(zpds_crypto STATIC ${ZPDS_CRYPTO_SOURCES})
