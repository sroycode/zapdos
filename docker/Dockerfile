# Docker File for Ubuntu based zapdos

FROM ubuntu:bionic

MAINTAINER S Roychowdhury <sroycode@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
ENV LANG C.UTF-8

RUN apt-get -y update -qq && \
    apt-get -y install locales apt-utils software-properties-common && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    add-apt-repository ppa:exonum/rocksdb && \
    add-apt-repository ppa:maarten-fonville/protobuf && \
    apt-get update -y -qq && \
    apt-get install -y \
    --no-install-recommends \
    git wget ccache cmake make libtool pkg-config g++ gcc autoconf automake curl jq lcov \
    protobuf-compiler vim-common libboost-all-dev libboost-all-dev \
    libcurl4-openssl-dev zlib1g-dev liblz4-dev libprotobuf-dev \
    libgoogle-glog-dev libgflags-dev libgoogle-perftools-dev libsnappy-dev libbz2-dev libz-dev libctemplate-dev \
    libtbb-dev libzstd-dev libxapian-dev librocksdb-dev wget && \
		wget -q http://ftp.us.debian.org/debian/pool/main/libc/libcrypto++/libcrypto++-dev_8.2.0-2_amd64.deb && \
		wget -q http://ftp.us.debian.org/debian/pool/main/libc/libcrypto++/libcrypto++-utils_8.2.0-2_amd64.deb && \
		wget -q http://ftp.us.debian.org/debian/pool/main/libc/libcrypto++/libcrypto++8_8.2.0-2_amd64.deb && \
		dpkg --install libcrypto++8_8.2.0-2_amd64.deb libcrypto++-dev_8.2.0-2_amd64.deb libcrypto++-utils_8.2.0-2_amd64.deb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* \
		libcrypto++8_8.2.0-2_amd64.deb libcrypto++-dev_8.2.0-2_amd64.deb libcrypto++-utils_8.2.0-2_amd64.deb

WORKDIR /app

RUN git clone https://github.com/sroycode/zapdos.git \
	&& cd zapdos \
	&& mkdir -p build \
	&& cd build \
	&& cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

COPY run_as_master.sh /usr/local/bin/run_as_master.sh
COPY run_as_slave.sh /usr/local/bin/run_as_slave.sh
COPY zapdos.conf /usr/local/etc/zapdos.conf

EXPOSE 9093
EXPOSE 9094
