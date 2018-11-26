#!/bin/bash

#### Set up Source Path #####

if [ -z ${ZAPDOS_SOURCE} ] ; then echo "PLEASE DEFINE ZAPDOS_SOURCE"; exit 1; fi
if [ -z ${ZAPDOS_TEMP} ] ; then
	export ZAPDOS_TEMP=${ZAPDOS_SOURCE}/thirdparty/src
fi
if [ -z ${ZAPDOS_TPSRC} ] ; then
	#export ZAPDOS_TPSRC=${ZAPDOS_TEMP}
	export ZAPDOS_TPSRC=/opt/backup
fi

CMAKE_CMD="cmake"

if [ -f "/etc/redhat-release" ] ; then
  . /opt/rh/devtoolset-7/enable
  export BOOST_ROOT=/opt/local/boost
  CMAKE_CMD="cmake3 -DBOOST_ROOT=${BOOST_ROOT}"
	CXXFLAGS="-std=c++11"
fi

export PATH=${PATH}:${ZAPDOS_SOURCE}/thirdparty/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export LD_RUN_PATH=${LD_RUN_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export CMAKE_CMD

mkdir -p ${ZAPDOS_SOURCE} ${ZAPDOS_TEMP}

#### Functions to get Sources #####

# Building gperftools 2.5

function build_gperftools () {
local MVERSION=2.5
local MSOURCEFILE=${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/gperftools-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/gperftools/tcmalloc.h ] ; then echo "gperftools already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	cd ${MWORKDIR}
	#patch < ${ZAPDOS_SOURCE}/scripts/gperftools-${MVERSION}.patch
	cd ${ZAPDOS_TEMP}
fi
cd ${MWORKDIR}
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Build protobuf 3.5.0

function build_protobuf () {
local MVERSION=3.5.0
local MSOURCEFILE=${ZAPDOS_TPSRC}/protobuf-cpp-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/protobuf-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/libprotobuf.a ] ; then
	if [ $# -eq 0 ] ; then echo "protobuf already installed"; return ; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/protobuf/releases/download/v${MVERSION}/protobuf-cpp-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building snappy 1.1.7

function build_snappy () {
local MVERSION=1.1.7
local MSOURCEFILE=${ZAPDOS_TPSRC}/snappy-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/snappy-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/snappy.h ] ; then 
	if [ $# -eq 0 ] ; then echo "snappy already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/snappy/archive/1.1.7.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty ..  && make && make install

}


# Building leveldb 1.20

function build_leveldb () {
build_rocks_compression 1
build_snappy 1
local MVERSION=1.20
local MSOURCEFILE=${ZAPDOS_TPSRC}/leveldb-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/leveldb-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/leveldb/db.h ] ; then
	if [ $# -ne 0 ] ; then echo "leveldb already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/leveldb/archive/v${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
PLATFORM_CXXFLAGS="-I ${ZAPDOS_SOURCE}/thirdparty/include" \
PLATFORM_LDFLAGS="-L ${ZAPDOS_SOURCE}/thirdparty/lib -L${ZAPDOS_SOURCE}/thirdparty/lib64" \
make \
&& cp out-static/libleveldb.a ${ZAPDOS_SOURCE}/thirdparty/lib/ \
&& cp -r include/leveldb ${ZAPDOS_SOURCE}/thirdparty/include/leveldb
}

# Building rocksdb 5.8

function build_rocksdb () {
build_rocks_compression 1
build_snappy 1
local MVERSION=5.8
local MSOURCEFILE=${ZAPDOS_TPSRC}/rocksdb-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/rocksdb-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/rocksdb/db.h ] ; then echo "rocksdb already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/facebook/rocksdb/archive/rocksdb-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mv rocksdb-rocksdb-${MVERSION} rocksdb-${MVERSION}
fi
cd ${MWORKDIR}
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
CFLAGS="-I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib -L${ZAPDOS_SOURCE}/thirdparty/lib64" \
INSTALL_PATH=${ZAPDOS_SOURCE}/thirdparty/ make static_lib \
&& INSTALL_PATH=${ZAPDOS_SOURCE}/thirdparty/ make install
}

# Building compression libs to link rocksdb
function build_rocks_compression () {
build_rocks_compression_zlib $1
build_rocks_compression_bzip2 $1
build_rocks_compression_lz4 $1
}

# Building compression libz to link rocksdb
function build_rocks_compression_zlib () {
local MVERSION=1.2.11
local MSOURCEFILE=${ZAPDOS_TPSRC}/zlib-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/zlib-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/zlib.h ] ; then
	if [ $# -eq 0 ] ; then echo "zlib already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "http://zlib.net/zlib-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CFLAGS='-fPIC' ./configure --static --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building compression bzip2 to link rocksdb
function build_rocks_compression_bzip2 () {
local MVERSION=1.0.6
local MSOURCEFILE=${ZAPDOS_TPSRC}/bzip2-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/bzip2-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/bzlib.h ] ; then
	if [ $# -eq 0 ] ; then echo "bz2 already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "http://distfiles.gentoo.org/distfiles/bzip2-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_SOURCE}/thirdparty
}

# Building compression lz4 

function build_rocks_compression_lz4 () {
local MVERSION=1.8.3
local MSOURCEFILE=${ZAPDOS_TPSRC}/lz4-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/lz4-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/liblz4.a ] ; then
	if [ $# -eq 0 ] ; then echo "lz4 already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/lz4/lz4/archive/v${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
make CFLAGS='-fPIC' && make install prefix=${ZAPDOS_SOURCE}/thirdparty
}

# Building googletest git-f1a87d7

function build_googletest () {
local MVERSION=git-f1a87d7
local MSOURCEFILE=${ZAPDOS_TPSRC}/googletest-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/googletest-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/gtest/gtest.h ] ; then echo "googletest already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/google/googletest.git googletest-${MVERSION}
		cd ${MWORKDIR}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} googletest-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty \
..  && make && make install
}

# Building ctemplate git-4b7e6c5

function build_ctemplate () {
local MVERSION=git-4b7e6c5
local MSOURCEFILE=${ZAPDOS_TPSRC}/ctemplate-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/ctemplate-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig/libctemplate.pc ] ; then echo "ctemplate already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/OlafvdSpek/ctemplate.git  ctemplate-${MVERSION}
		cd ${MWORKDIR}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		sh autogen.sh
		if [ `uname` = 'Darwin' ] ; then
			sed -i .bkp 's/env python2/env python2.7/' src/htmlparser/generate_fsm.py
		fi
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} ctemplate-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
cd ${MWORKDIR}/
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make CXXFLAGS="$CXXFLAGS -std=c++11" && make install
}

# Building xapian 1.4.6

function build_xapian () {
local MVERSION=1.4.6
local MSOURCEFILE=${ZAPDOS_TPSRC}/xapian-${MVERSION}.tar
local MWORKDIR=${ZAPDOS_TEMP}/xapian-core-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/xapian.h ] ; then
	if [ $# -eq 0 ] ; then echo "xapian already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE}.xz "http://oligarchy.co.uk/xapian/${MVERSION}/xapian-core-${MVERSION}.tar.xz"
		unxz ${MSOURCEFILE}.xz
	fi
	cd ${ZAPDOS_TEMP}
	tar -xf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CFLAGS="-O2 -I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib" \
./configure --enable-static=yes -enable-64bit-docid=yes --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building Libsodium 1.0.16

function build_sodium () {
local MVERSION=1.0.16
local MSOURCEFILE=${ZAPDOS_TPSRC}/libsodium-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/libsodium-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/sodium.h ] ; then echo "libsodium already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/jedisct1/libsodium/releases/download/${MVERSION}/libsodium-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CFLAGS="-O2 -I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib" \
./configure --enable-static=yes --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building Cityhash git-8af9b8c

function build_cityhash () {
local MVERSION=git-8af9b8c
local MSOURCEFILE=${ZAPDOS_TPSRC}/cityhash-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/cityhash-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/city.h ] ; then echo "cityhash already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/google/cityhash.git cityhash-${MVERSION}
		cd ${MWORKDIR}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} cityhash-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
cd ${MWORKDIR}
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building sparsehash 2.0.3

function build_sparsehash () {
local MVERSION=2.0.3
local MSOURCEFILE=${ZAPDOS_TPSRC}/sparsehash-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/sparsehash-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/sparsehash/type_traits.h ] ; then
	if [ $# -eq 0 ] ; then echo "sparsehash already installed"; fi ;
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/sparsehash/sparsehash/archive/sparsehash-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mv sparsehash-sparsehash-${MVERSION} sparsehash-${MVERSION}
fi
cd ${MWORKDIR}
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building protozero 1.6.3

function build_protozero () {
local MVERSION=1.6.3
local MSOURCEFILE=${ZAPDOS_TPSRC}/protozero-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/protozero-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/protozero/version.hpp ] ; then echo "protozero already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/mapbox/protozero/archive/v${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
mkdir -p ${MWORKDIR}/build
cd ${MWORKDIR}/build
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib \
DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib \
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty \
..  && make && cp -r ../include/* ${ZAPDOS_SOURCE}/thirdparty/include/
}

# Building libosmium git-1bb69ec

function build_libosmium () {
build_rocks_compression 1
build_sparsehash 1
build_protozero 1
local MVERSION=git-1bb69ec
local MSOURCEFILE=${ZAPDOS_TPSRC}/libosmium-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/libosmium-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/osmium/version.hpp ] ; then echo "libosmium already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/osmcode/libosmium.git libosmium-${MVERSION}
		cd ${MWORKDIR}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} libosmium-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
mkdir -p ${MWORKDIR}/build
cd ${MWORKDIR}/build
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib \
DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib \
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=Off -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty \
..  && make && cp -r ../include/* ${ZAPDOS_SOURCE}/thirdparty/include/
}

# Building libspatialindex git 4768bf3

function build_libspatialindex () {
local MVERSION=git-4768bf3
local MSOURCEFILE=${ZAPDOS_TPSRC}/libspatialindex-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/libspatialindex-${MVERSION}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/gtest/gtest.h ] ; then echo "libspatialindex already installed"; return ; fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone https://github.com/google/libspatialindex.git googletest-${MVERSION}
		cd ${MWORKDIR}
		git checkout "`echo ${MVERSION} | sed 's/git-//'`"
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} libspatialindex-${MVERSION}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
cd ${MWORKDIR}
sh autogen.sh
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
..  && make && make install
}

# Building stxxl git  0a80a8c 

function build_stxxl () {
local MVERSION=0a80a8c
local MWORKDIR=${ZAPDOS_TEMP}/stxxl
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/stxxl.h ] ; then echo "stxxl already installed"; return ; fi
if [ ! -d ${ZAPDOS_TEMP}/stxxl ] ; then
	cd ${ZAPDOS_TEMP}
	git clone https://github.com/stxxl/stxxl.git stxxl
	cd stxxl
	git checkout ${MVERSION}
	mkdir build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty \
..  && make && make install
}


### Control ####

build_gperftools
build_protobuf
build_snappy
build_rocks_compression
#build_leveldb
build_rocksdb
build_googletest
build_ctemplate
build_xapian
build_cityhash
build_stxxl
