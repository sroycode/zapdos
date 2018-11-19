# Docker File for Centos based zapdos

FROM sroycode/zapdos_build
MAINTAINER S Roychowdhury <sroycode@gmail.com>

ENV LANG C

# Build environment
ENV BOOST_ROOT /opt/local/boost
ENV ZAPDOS_SOURCE /app/zapdos
ENV ZAPDOS_TEMP /app/zapdos_temp
ENV ZAPDOS_DEPS /opt/local/zapdos
ENV ZAPDOS_TPSRC /opt/backup

ENV PATH ${PATH}:${ZAPDOS_DEPS}/thirdparty/bin
ENV CPPFLAGS -I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include
ENV CXXFLAGS -I/opt/local/include -I/usr/local/include -I${ZAPDOS_DEPS}/thirdparty/include
ENV LDFLAGS -L/opt/local/lib -L/usr/local/lib -L${ZAPDOS_DEPS}/thirdparty/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ZAPDOS_DEPS}/thirdparty/lib:${ZAPDOS_DEPS}/thirdparty/lib64
ENV PKG_CONFIG_PATH ${PKG_CONFIG_PATH}:${ZAPDOS_DEPS}/thirdparty/lib/pkgconfig:/usr/local/lib/pkgconfig
ENV CXXFLAGS -std=c++11

### MAIN 

ENV MVERSION 0.4.1
ENV MWORKDIR ${ZAPDOS_TEMP}/zapdos-${MVERSION}

RUN git clone https://github.com/sroycode/zapdos.git ${MWORKDIR} \
	&& cd zapdos-${MVERSION} \
	&& git checkout v${MVERSION} \
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
