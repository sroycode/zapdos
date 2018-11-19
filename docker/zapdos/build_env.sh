#!/bin/bash


# Build environment
export BOOST_ROOT="/opt/local/boost"
export ZAPDOS_SOURCE="/app/zapdos"
export ZAPDOS_TEMP="/app/zapdos_temp"
export ZAPDOS_DEPS="/opt/local/zapdos"
export ZAPDOS_TPSRC="/opt/backup"
export PATH="${PATH}:${ZAPDOS_DEPS}/thirdparty/bin"
export CPPFLAGS="-I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include"
export CXXFLAGS="-I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include"
export LDFLAGS="-L/opt/local/lib -L/usr/local/lib -L${ZAPDOS_DEPS}/thirdparty/lib"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${ZAPDOS_DEPS}/thirdparty/lib:${ZAPDOS_DEPS}/thirdparty/lib64"
export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig:/usr/local/lib/pkgconfig"
export CXXFLAGS="-std=c++11"

# Dependencies

# Build boost 1.65 from source
mkdir -p ${BOOST_ROOT} ${ZAPDOS_TEMP}/scripts ${ZAPDOS_TPSRC} \
	&& wget -q -O ${ZAPDOS_TPSRC}/boost_1_67_0.tar.bz2 https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2 \
	&& cd ${ZAPDOS_TEMP} \
	&& source /opt/rh/devtoolset-7/enable \
	&& tar -jxf ${ZAPDOS_TPSRC}/boost_1_67_0.tar.bz2 \
	&& cd boost_1_67_0 \
	&& bash bootstrap.sh --prefix=${BOOST_ROOT} \
	&& ( ./b2 install ; echo OK )
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_gperftools 
export MVERSION=2.5
export MSOURCEFILE=${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/gperftools-${MVERSION}
wget -q -O ${MSOURCEFILE} \
	https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& ./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_rocks_compression_zlib 
export MVERSION=1.2.11
export MSOURCEFILE=${ZAPDOS_TPSRC}/zlib-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/zlib-${MVERSION}
wget -q -O ${MSOURCEFILE} http://zlib.net/zlib-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS='-fPIC' ./configure --static --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_rocks_compression_bzip2 
export MVERSION=1.0.6
export MSOURCEFILE=${ZAPDOS_TPSRC}/bzip2-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/bzip2-${MVERSION}
wget -O ${MSOURCEFILE} http://distfiles.gentoo.org/distfiles/bzip2-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_DEPS}/thirdparty
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_rocks_compression_lz4
export MVERSION=1.8.3
export MSOURCEFILE=${ZAPDOS_TPSRC}/lz4-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/lz4-${MVERSION}
wget -O ${MSOURCEFILE} https://github.com/lz4/lz4/archive/v${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_DEPS}/thirdparty
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_snappy
export MVERSION=1.1.7
export MSOURCEFILE=${ZAPDOS_TPSRC}/snappy-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/snappy-${MVERSION}
wget -O ${MSOURCEFILE} https://github.com/google/snappy/archive/${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& mkdir -p ${MWORKDIR}/build \
	&& cd ${MWORKDIR}/build \
	&& source /opt/rh/devtoolset-7/enable \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	cmake3 -DBOOST_ROOT=${BOOST_ROOT} \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_DEPS}/thirdparty ..  && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_gperftools 
export MVERSION=2.5
export MSOURCEFILE=${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/gperftools-${MVERSION}
wget -q -O ${MSOURCEFILE} \
	https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& ./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_googletest
export MVERSION=git-f1a87d7
cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/google/googletest.git googletest-${MVERSION} \
	&& cd googletest-${MVERSION} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& mkdir -p build \
	&& cd build \
	&& source /opt/rh/devtoolset-7/enable \
	&& cmake3 -DBOOST_ROOT=${BOOST_ROOT} \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_DEPS}/thirdparty ..  && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_ctemplate
export MVERSION=git-4b7e6c5
export MWORKDIR=${ZAPDOS_TEMP}/ctemplate-${MVERSION}
cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/OlafvdSpek/ctemplate.git  ctemplate-${MVERSION} \
	&& cd ctemplate-${MVERSION} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& source /opt/rh/devtoolset-7/enable \
	&& sh autogen.sh \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make CXXFLAGS="$CXXFLAGS -std=c++11" && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_sodium 
export MVERSION=1.0.16
export MSOURCEFILE=${ZAPDOS_TPSRC}/libsodium-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/libsodium-${MVERSION}
wget -O ${MSOURCEFILE} https://github.com/jedisct1/libsodium/releases/download/${MVERSION}/libsodium-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& sh autogen.sh \
	&& CFLAGS="-O2 -I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --enable-static=yes --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_cityhash 
export MVERSION=git-8af9b8c
export MSOURCEFILE=${ZAPDOS_TPSRC}/cityhash-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/cityhash-${MVERSION}
cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/google/cityhash.git cityhash-${MVERSION} \
	&& cd ${MWORKDIR} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_rocksdb
export MVERSION=5.8
export MSOURCEFILE=${ZAPDOS_TPSRC}/rocksdb-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/rocksdb-${MVERSION}
wget -O ${MSOURCEFILE} https://github.com/facebook/rocksdb/archive/rocksdb-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& mv rocksdb-rocksdb-${MVERSION} rocksdb-${MVERSION} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib -L${ZAPDOS_DEPS}/thirdparty/lib64" \
	INSTALL_PATH=${ZAPDOS_DEPS}/thirdparty/ make static_lib \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib -L${ZAPDOS_DEPS}/thirdparty/lib64" \
	INSTALL_PATH=${ZAPDOS_DEPS}/thirdparty/ make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_xapian 
export MVERSION=1.4.7
export MSOURCEFILE=${ZAPDOS_TPSRC}/xapian-${MVERSION}.tar
export MWORKDIR=${ZAPDOS_TEMP}/xapian-core-${MVERSION}
wget -O ${MSOURCEFILE}.xz http://oligarchy.co.uk/xapian/${MVERSION}/xapian-core-${MVERSION}.tar.xz \
	&& cd ${ZAPDOS_TEMP} \
	&& unxz ${MSOURCEFILE}.xz \
	&& tar -xf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CXXFLAGS="-O2 -I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --enable-static=yes -enable-64bit-docid=yes --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# build_protobuf 
export MVERSION=3.5.0
export MSOURCEFILE=${ZAPDOS_TPSRC}/protobuf-cpp-${MVERSION}.tar.gz
export MWORKDIR=${ZAPDOS_TEMP}/protobuf-${MVERSION}
wget -O ${MSOURCEFILE} \
	https://github.com/google/protobuf/releases/download/v${MVERSION}/protobuf-cpp-${MVERSION}.tar.gz \
	&& cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install
if [ $? -ne 0 ] ; then exit 1 ; fi

# delete temp
rm -Rf ${ZAPDOS_TEMP}/* ${ZAPDOS_TPSRC}/*
