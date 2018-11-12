# Docker File for Centos based zapdos

FROM centos:centos7
MAINTAINER S Roychowdhury <sroycode@gmail.com>

ENV LANG C

RUN yum install -y deltarpm wget yum-utils
RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

RUN yum -y update && yum clean all && yum -y install centos-release-scl 
RUN yum -y install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils \
	cmake3 glog-devel libunwind-devel libicu-devel libcurl-devel \
	wget curl python2-httpie autoconf automake libtool ccache \
	bzip2 zlib bzip2-devel zlib-devel tbb-devel glog-devel gflags-devel which \
	file make git

ENV BOOST_ROOT /opt/local/boost
ENV ZAPDOS_SOURCE /app/zapdos
ENV ZAPDOS_TEMP /app/zapdos_temp
ENV ZAPDOS_DEPS /opt/local/zapdos
ENV ZAPDOS_TPSRC /opt/backup

# Build boost 1.65 from source
RUN mkdir -p ${BOOST_ROOT} ${ZAPDOS_TEMP}/scripts ${ZAPDOS_TPSRC} 
RUN wget -q -O ${ZAPDOS_TPSRC}/boost_1_67_0.tar.bz2 https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2

RUN cd ${ZAPDOS_TEMP} \
	&& source /opt/rh/devtoolset-7/enable \
	&& tar -jxf ${ZAPDOS_TPSRC}/boost_1_67_0.tar.bz2 \
	&& cd boost_1_67_0 \
	&& bash bootstrap.sh --prefix=${BOOST_ROOT} \
	&& ( ./b2 install ; echo OK )

# Build environment

ENV PATH ${PATH}:${ZAPDOS_DEPS}/thirdparty/bin
ENV CPPFLAGS -I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include
ENV CXXFLAGS -I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include
ENV LDFLAGS -L/opt/local/lib -L/usr/local/lib -L${ZAPDOS_DEPS}/thirdparty/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ZAPDOS_DEPS}/thirdparty/lib:${ZAPDOS_DEPS}/thirdparty/lib64
ENV PKG_CONFIG_PATH ${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig:/usr/local/lib/pkgconfig
ENV CXXFLAGS -std=c++11

# Dependencies


# build_gperftools 

ENV MVERSION 2.5
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/gperftools-${MVERSION}
RUN wget -q -O ${MSOURCEFILE} \
	https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& ./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_rocks_compression_zlib 

ENV MVERSION 1.2.11
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/zlib-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/zlib-${MVERSION}
RUN wget -q -O ${MSOURCEFILE} http://zlib.net/zlib-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS='-fPIC' ./configure --static --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_rocks_compression_bzip2 

ENV MVERSION 1.0.6
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/bzip2-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/bzip2-${MVERSION}
RUN wget -O ${MSOURCEFILE} http://distfiles.gentoo.org/distfiles/bzip2-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_DEPS}/thirdparty

# build_rocks_compression_lz4

ENV MVERSION 1.8.3
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/lz4-${MVERSION}.tar.gz
ENV MWORKDIR=${ZAPDOS_TEMP}/lz4-${MVERSION}
RUN wget -O ${MSOURCEFILE} https://github.com/lz4/lz4/archive/v${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_DEPS}/thirdparty

# build_snappy

ENV MVERSION 1.1.7
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/snappy-${MVERSION}.tar.gz
ENV MWORKDIR=${ZAPDOS_TEMP}/snappy-${MVERSION}
RUN wget -O ${MSOURCEFILE} https://github.com/google/snappy/archive/${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& mkdir -p ${MWORKDIR}/build \
	&& cd ${MWORKDIR}/build \
	&& source /opt/rh/devtoolset-7/enable \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	cmake3 -DBOOST_ROOT=${BOOST_ROOT} \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_DEPS}/thirdparty ..  && make && make install

# build_gperftools

ENV MVERSION 2.5
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/gperftools-${MVERSION}

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& make CFLAGS='-fPIC' && make install PREFIX=${ZAPDOS_DEPS}/thirdparty

# build_gperftools 

ENV MVERSION 2.5
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/gperftools-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/gperftools-${MVERSION}
RUN wget -q -O ${MSOURCEFILE} \
	https://github.com/gperftools/gperftools/releases/download/gperftools-${MVERSION}/gperftools-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& ./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_rocks_compression_zlib 

ENV MVERSION 1.2.11
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/zlib-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/zlib-${MVERSION}
RUN wget -q -O ${MSOURCEFILE} http://zlib.net/zlib-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS='-fPIC' ./configure --static --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_rocks_compression_bzip2 

ENV MVERSION 1.0.6
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/bzip2-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/bzip2-${MVERSION}
# build_googletest

ENV MVERSION git-f1a87d7

RUN cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/google/googletest.git googletest-${MVERSION} \
	&& cd googletest-${MVERSION} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& mkdir -p build \
	&& cd build \
	&& source /opt/rh/devtoolset-7/enable \
	&& cmake3 -DBOOST_ROOT=${BOOST_ROOT} \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${ZAPDOS_DEPS}/thirdparty ..  && make && make install

# build_ctemplate

ENV MVERSION git-4b7e6c5
ENV MWORKDIR ${ZAPDOS_TEMP}/ctemplate-${MVERSION}

RUN cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/OlafvdSpek/ctemplate.git  ctemplate-${MVERSION} \
	&& cd ctemplate-${MVERSION} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& source /opt/rh/devtoolset-7/enable \
	&& sh autogen.sh \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make CXXFLAGS="$CXXFLAGS -std=c++11" && make install

# build_sodium 

ENV MVERSION 1.0.16
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/libsodium-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/libsodium-${MVERSION}

RUN wget -O ${MSOURCEFILE} https://github.com/jedisct1/libsodium/releases/download/${MVERSION}/libsodium-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& sh autogen.sh \
	&& CFLAGS="-O2 -I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --enable-static=yes --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_cityhash 

ENV MVERSION git-8af9b8c
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/cityhash-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/cityhash-${MVERSION}

RUN cd ${ZAPDOS_TEMP} \
	&& git clone https://github.com/google/cityhash.git cityhash-${MVERSION} \
	&& cd ${MWORKDIR} \
	&& git checkout "`echo ${MVERSION} | sed 's/git-//'`" \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

# build_rocksdb

ENV MVERSION 5.8
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/rocksdb-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/rocksdb-${MVERSION}

RUN wget -O ${MSOURCEFILE} https://github.com/facebook/rocksdb/archive/rocksdb-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& mv rocksdb-rocksdb-${MVERSION} rocksdb-${MVERSION} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig \
	CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib -L${ZAPDOS_DEPS}/thirdparty/lib64" \
	INSTALL_PATH=${ZAPDOS_DEPS}/thirdparty/ make static_lib \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib -L${ZAPDOS_DEPS}/thirdparty/lib64" \
	INSTALL_PATH=${ZAPDOS_DEPS}/thirdparty/ make install

# build_xapian 

ENV MVERSION 1.4.7
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/xapian-${MVERSION}.tar
ENV MWORKDIR ${ZAPDOS_TEMP}/xapian-core-${MVERSION}

RUN wget -O ${MSOURCEFILE}.xz http://oligarchy.co.uk/xapian/${MVERSION}/xapian-core-${MVERSION}.tar.xz

RUN cd ${ZAPDOS_TEMP} \
	&& unxz ${MSOURCEFILE}.xz \
	&& tar -xf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CXXFLAGS="-O2 -I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --enable-static=yes -enable-64bit-docid=yes --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install


# build_protobuf 

ENV MVERSION 3.5.0
ENV MSOURCEFILE ${ZAPDOS_TPSRC}/protobuf-cpp-${MVERSION}.tar.gz
ENV MWORKDIR ${ZAPDOS_TEMP}/protobuf-${MVERSION}

RUN wget -O ${MSOURCEFILE} \
	https://github.com/google/protobuf/releases/download/v${MVERSION}/protobuf-cpp-${MVERSION}.tar.gz

RUN cd ${ZAPDOS_TEMP} \
	&& tar -zxf ${MSOURCEFILE} \
	&& cd ${MWORKDIR} \
	&& source /opt/rh/devtoolset-7/enable \
	&& CFLAGS="-I ${ZAPDOS_DEPS}/thirdparty/include -L${ZAPDOS_DEPS}/thirdparty/lib" \
	./configure --prefix=${ZAPDOS_DEPS}/thirdparty && make && make install

RUN yum -y install libpqxx-devel nginx

RUN rm -Rf ${ZAPDOS_TEMP}/* ${ZAPDOS_TPSRC}/*

### MAIN 

# ENV MVERSION 1.0.0 # target
ENV MWORKDIR ${ZAPDOS_TEMP}/zapdos-${MVERSION}

RUN git clone https://github.com/sroycode/zapdos.git ${MWORKDIR} \
	&& cd ${MWORKDIR} \
	&& ln -s ${ZAPDOS_DEPS}/thirdparty ${MWORKDIR}/thirdparty \
	&& mkdir -p ${MWORKDIR}/build \
	&& cd ${MWORKDIR}/build \
	&& source /opt/rh/devtoolset-7/enable \
	&& cmake3 -DBOOST_ROOT=${BOOST_ROOT} -DCMAKE_INSTALL_PREFIX=${ZAPDOS_DEPS}/thirdparty .. && make && make install

COPY run_as_master.sh ${ZAPDOS_SOURCE}/run_as_master.sh
COPY run_as_slave.sh ${ZAPDOS_SOURCE}/run_as_slave.sh
COPY zapdos.conf ${ZAPDOS_SOURCE}/zapdos.conf

EXPOSE 9091
EXPOSE 9092
