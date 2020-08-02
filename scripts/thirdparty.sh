#!/bin/bash

#### Set up Source Path #####

if [ -z ${ZAPDOS_SOURCE} ] ; then echo "PLEASE DEFINE ZAPDOS_SOURCE"; exit 1; fi
if [ -z ${ZAPDOS_TEMP} ] ; then
	export ZAPDOS_TEMP=${ZAPDOS_SOURCE}/thirdparty/src
fi
if [ -z ${ZAPDOS_TPSRC} ] ; then
	#export ZAPDOS_TPSRC=${ZAPDOS_TEMP}
	export ZAPDOS_TPSRC=${HOME}/opt/backup
fi

CMAKE_CMD="cmake -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC"

if [ -f "/etc/redhat-release" ] ; then
  . /opt/rh/devtoolset-7/enable
  export BOOST_ROOT=/opt/local/boost
  CMAKE_CMD="cmake3 -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC -DBOOST_ROOT=${BOOST_ROOT}"
  CXXFLAGS="-std=c++11"
fi


export PATH=${PATH}:${ZAPDOS_SOURCE}/thirdparty/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export LD_RUN_PATH=${LD_RUN_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib:${ZAPDOS_SOURCE}/thirdparty/lib64
export CMAKE_CMD

mkdir -p ${ZAPDOS_SOURCE} ${ZAPDOS_TEMP} ${ZAPDOS_TPSRC}

#### Functions to get Sources #####

# Building gperftools 2.5

function build_gperftools () {
local MVERSION=2.5
local MPROGNAME=gperftools-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/gperftools/tcmalloc.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz"
		if [ $? -ne 0 ] ; then echo "$MWORKDIR wget failed"; exit 1 ; fi
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	cd ${MWORKDIR}
	cd ${ZAPDOS_TEMP}
fi
cd ${MWORKDIR}
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Build protobuf 3.5.0

function build_protobuf () {
local MVERSION=3.5.0
local MPROGNAME=protobuf-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/protobuf-all-${MVERSION}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/libprotobuf.a ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/protobuf/releases/download/v${MVERSION}/protobuf-all-${MVERSION}.tar.gz"
		if [ $? -ne 0 ] ; then echo "$MWORKDIR wget failed"; exit 1 ; fi
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building snappy 1.1.7

function build_snappy () {
local MVERSION=1.1.7
local MPROGNAME=snappy-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/snappy.h ] ; then 
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/snappy/archive/1.1.7.tar.gz"
		if [ $? -ne 0 ] ; then echo "$MWORKDIR wget failed"; exit 1 ; fi
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty ..  && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi

}


# Building leveldb 1.20

function build_leveldb () {
build_snappy 1
local MVERSION=1.20
local MPROGNAME=leveldb-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/leveldb/db.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/google/leveldb/archive/v${MVERSION}.tar.gz"
		if [ $? -ne 0 ] ; then echo "$MWORKDIR wget failed"; exit 1 ; fi
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
PLATFORM_CXXFLAGS="-I ${ZAPDOS_SOURCE}/thirdparty/include -fPIC" \
PLATFORM_LDFLAGS="-L ${ZAPDOS_SOURCE}/thirdparty/lib -L${ZAPDOS_SOURCE}/thirdparty/lib64" \
make \
&& cp out-static/libleveldb.a ${ZAPDOS_SOURCE}/thirdparty/lib/ \
&& cp -r include/leveldb ${ZAPDOS_SOURCE}/thirdparty/include/leveldb
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}


# Building compression libs to link rocksdb
function build_rocks_compression () {
build_rocks_compression_zlib $1
build_rocks_compression_bzip2 $1
build_rocks_compression_lz4 $1
build_rocks_compression_zstd $1
}

# Building compression libz to link rocksdb
function build_rocks_compression_zlib () {
local MVERSION=1.2.11
local MSOURCEFILE=${ZAPDOS_TPSRC}/zlib-${MVERSION}.tar.gz
local MPROGNAME=zlib-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/zlib.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
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
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building compression bzip2 to link rocksdb
function build_rocks_compression_bzip2 () {
local MVERSION=1.0.6
local MSOURCEFILE=${ZAPDOS_TPSRC}/bzip2-${MVERSION}.tar.gz
local MPROGNAME=bzip2-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/bzlib.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
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
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building compression lz4 

function build_rocks_compression_lz4 () {
local MVERSION=1.8.3
local MSOURCEFILE=${ZAPDOS_TPSRC}/lz4-${MVERSION}.tar.gz
local MPROGNAME=lz4-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/liblz4.a ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
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
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building compression zstd 1.3.8

function build_rocks_compression_zstd () {
local MVERSION=1.3.8
local MSOURCEFILE=${ZAPDOS_TPSRC}/zstd-${MVERSION}.tar.gz
local MPROGNAME=zstd-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/libzstd.a ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/facebook/zstd/archive/v${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
make CFLAGS='-fPIC' && make install prefix=${ZAPDOS_SOURCE}/thirdparty
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building rocksdb 5.8.8

function build_rocksdb () {
build_rocks_compression 1
build_snappy 1
local MVERSION=5.8.8
local MSOURCEFILE=${ZAPDOS_TPSRC}/rocksdb-${MVERSION}.tar.gz
local MPROGNAME=rocksdb-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/rocksdb/db.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/facebook/rocksdb/archive/rocksdb-${MVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mv rocksdb-rocksdb-${MVERSION} ${MPROGNAME}
fi
cd ${MWORKDIR}
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
CFLAGS="-I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib -L${ZAPDOS_SOURCE}/thirdparty/lib64 -fPIC" \
CXXFLAGS="-I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib -L${ZAPDOS_SOURCE}/thirdparty/lib64 -fPIC" \
INSTALL_PATH=${ZAPDOS_SOURCE}/thirdparty/ make static_lib \
&& INSTALL_PATH=${ZAPDOS_SOURCE}/thirdparty/ make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building googletest git-f1a87d7

function build_googletest () {
local MVERSION=git-f1a87d7
local MPROGNAME=googletest-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/gtest/gtest.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone --single-branch -b master https://github.com/google/googletest.git ${MPROGNAME}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git clone failed"; exit 1 ; fi
		cd ${MWORKDIR} && git checkout -b "`echo ${MVERSION} | sed 's/git-//'`" && cd ${ZAPDOS_TEMP}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git checkout failed"; exit 1 ; fi
		tar -zcf ${MSOURCEFILE} ${MPROGNAME}
	else
		tar -zxf ${MSOURCEFILE}
	fi
	mkdir -p ${MWORKDIR}/build
fi
cd ${MWORKDIR}/build
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
${CMAKE_CMD} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_SOURCE}/thirdparty \
..  && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building ctemplate git-4b7e6c5

function build_ctemplate () {
local MVERSION=git-4b7e6c5
local MPROGNAME=ctemplate-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig/libctemplate.pc ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone --single-branch -b master https://github.com/OlafvdSpek/ctemplate.git  ${MPROGNAME}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git clone failed"; exit 1 ; fi
		cd ${MWORKDIR} && git checkout -b "`echo ${MVERSION} | sed 's/git-//'`" && cd ${ZAPDOS_TEMP}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git checkout failed"; exit 1 ; fi

		cd ${MWORKDIR}
		sh autogen.sh
		if [ `uname` = 'Darwin' ] ; then
			sed -i .bkp 's/env python2/env python2.7/' src/htmlparser/generate_fsm.py
		fi
		cd ${ZAPDOS_TEMP}
		tar -zcf ${MSOURCEFILE} ${MPROGNAME}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
cd ${MWORKDIR}/
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_SOURCE}/thirdparty/lib/pkgconfig \
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make CXXFLAGS="$CXXFLAGS -std=c++14" && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building Cityhash git-8af9b8c

function build_cityhash () {
local MVERSION=git-8af9b8c
local MPROGNAME=cityhash-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/city.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	cd ${ZAPDOS_TEMP}
	if [ ! -f ${MSOURCEFILE} ] ; then
		git clone --single-branch -b master https://github.com/google/cityhash.git ${MPROGNAME}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git clone failed"; exit 1 ; fi
		cd ${MWORKDIR} && git checkout -b "`echo ${MVERSION} | sed 's/git-//'`" && cd ${ZAPDOS_TEMP}
		if [ $? -ne 0 ] ; then echo "${MPROGNAME} git checkout failed"; exit 1 ; fi
		tar -zcf ${MSOURCEFILE} ${MPROGNAME}
	else
		tar -zxf ${MSOURCEFILE}
	fi
fi
cd ${MWORKDIR}
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building sparsehash 2.0.3

function build_sparsehash () {
local MVERSION=2.0.3
local MPROGNAME=sparsehash-${MVERSION}
local MSOURCEFILE=${ZAPDOS_TPSRC}/${MPROGNAME}.tar.gz
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/sparsehash/type_traits.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/sparsehash/sparsehash/archive/sparsehash-${MVERSION}.tar.gz"
		if [ $? -ne 0 ] ; then echo "$MWORKDIR wget failed"; exit 1 ; fi
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
	mv sparsehash-sparsehash-${MVERSION} ${MPROGNAME}
fi
cd ${MWORKDIR}
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
./configure --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
if [ $? -ne 0 ] ; then echo "$MWORKDIR install failed"; exit 1 ; fi
}

# Building xapian 1.4.10

function build_xapian () {
local MVERSION=1.4.10
local MSOURCEFILE=${ZAPDOS_TPSRC}/xapian-${MVERSION}.tar
local MPROGNAME=xapian-core-${MVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/xapian.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
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
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
CFLAGS="-O2 -I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib" \
./configure --enable-static=yes -enable-64bit-docid=yes --prefix=${ZAPDOS_SOURCE}/thirdparty && make && make install
}

# Building cryptopp 8.2.0

function build_cryptopp () {
local MVERSION=8.2.0
local UVERSION="`echo ${MVERSION} | sed 's/\./_/g'`"
local MSOURCEFILE=${ZAPDOS_TPSRC}/CRYPTOPP_${UVERSION}.tar.gz
local MPROGNAME=cryptopp-CRYPTOPP_${UVERSION}
local MWORKDIR=${ZAPDOS_TEMP}/${MPROGNAME}
if [ -f ${ZAPDOS_SOURCE}/thirdparty/include/crypto++/cryptlib.h ] ; then
	if [ $# -eq 0 ] ; then echo "${MPROGNAME} already installed"; fi
	return;
fi
if [ ! -d ${MWORKDIR} ] ; then
	if [ ! -f ${MSOURCEFILE} ] ; then
		wget -O ${MSOURCEFILE} "https://github.com/weidai11/cryptopp/archive/CRYPTOPP_${UVERSION}.tar.gz"
	fi
	cd ${ZAPDOS_TEMP}
	tar -zxf ${MSOURCEFILE}
fi
cd ${MWORKDIR}
CXXFLAGS="$CXXFLAGS -fPIC" CFLAGS="$CFLAGS -fPIC" \
CFLAGS="-O2 -I ${ZAPDOS_SOURCE}/thirdparty/include -L${ZAPDOS_SOURCE}/thirdparty/lib" \
PREFIX="${ZAPDOS_SOURCE}/thirdparty" \
make install \
&& ln -s ${ZAPDOS_SOURCE}/thirdparty/include/cryptopp ${ZAPDOS_SOURCE}/thirdparty/include/crypto++ 
}
### Control ####

if [ $# -ne 1 ] ; then echo "Usage $0: cryptopp" ; exit 1; fi

for REQS in wget cmake ; do
if [ ! "`which ${REQS}`" ] ; then echo "${REQS} not installed"; exit 1; fi
done

if [ "$1" = "gperftools" ] ; then build_gperftools ;
elif [ "$1" = "protobuf" ] ; then build_protobuf ;
elif [ "$1" = "snappy" ] ; then build_snappy ;
elif [ "$1" = "leveldb" ] ; then build_leveldb ;
elif [ "$1" = "rocksdb" ] ; then build_rocksdb ;
elif [ "$1" = "googletest" ] ; then build_googletest ;
elif [ "$1" = "ctemplate" ] ; then build_ctemplate ;
elif [ "$1" = "cityhash" ] ; then build_cityhash ;
elif [ "$1" = "sparsehash" ] ; then build_sparsehash ;
elif [ "$1" = "xapian" ] ; then build_xapian ;
elif [ "$1" = "cryptopp" ] ; then build_cryptopp ;
else echo "Usage: $0 cryptopp" ; exit 1; fi
