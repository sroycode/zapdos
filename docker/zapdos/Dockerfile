# Docker File for Ubuntu based zapdos

FROM ubuntu:bionic

MAINTAINER S Roychowdhury <sroycode@gmail.com>

ENV DEBIAN_FRONTEND noninteractive
ENV LANG C.UTF-8

RUN apt-get -y update -qq && \
    apt-get -y install locales && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    add-apt-repository ppa:exonum/rocksdb && \
    add-apt-repository ppa:maarten-fonville/protobuf && \
    add-apt-repository ppa:xapian-backports/ppa && \
    apt-get update -y && \
    apt-get install -y \
    git wget ccache cmake make libtool pkg-config g++ gcc autoconf automake curl jq lcov \
    protobuf-compiler vim-common libboost-all-dev libboost-all-dev \
    libcurl4-openssl-dev zlib1g-dev liblz4-dev libprotobuf-dev \
    libgoogle-glog-dev libgflags-dev libgoogle-perftools-dev libsnappy-dev libbz2-dev libz-dev libctemplate-dev \
    libtbb-dev libzstd-dev libxapian-dev libsodium-dev libosmium-dev \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /tmp/* /var/tmp/*

WORKDIR /app

ENV MVERSION devel
ENV MWORKDIR zapdos-${MVERSION}

RUN git clone https://github.com/sroycode/zapdos.git ${MWORKDIR} \
	&& cd zapdos-${MVERSION} \
	&& git checkout v${MVERSION} \
	&& mkdir -p build \
	&& cd build \
	&& cmake .. && make && make install

COPY run_as_master.sh ${ZAPDOS_SOURCE}/run_as_master.sh
COPY run_as_slave.sh ${ZAPDOS_SOURCE}/run_as_slave.sh
COPY zapdos.conf ${ZAPDOS_SOURCE}/zapdos.conf

EXPOSE 9091
EXPOSE 9092
