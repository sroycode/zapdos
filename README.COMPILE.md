# Compile from Source

## Creating Development Environment

You need to install these tools using standard utils.
- cmake 3+,
- boost 1.62+
- clang++/gcc5 ( supporting C++11)
- autoconf automake etc ( for thirdparty tools )
- icu4c
- glog and gflags.
- libcurl 7.0+ 
- httpie for testing


The ones below can be compiled from source if you need,there is a helper `scripts/thirdparty.sh`
- gperftools 2.5
- protobuf 3.5.0
- snappy 1.1.7
- rocksdb compression libs zlib , bzip2 , lz4 - preferably static (*)
- rocksdb 5.8  ( script installs above headers ) (*)
- libsodium 1.0.16
- cityhash from git
- libosmium 2.14.2 ( requires protozero and sparsehash )

Items marked with asterix (*) should preferably be compiled from source
On mac+homebrew and ubuntu+apt default versions work fine.  For Centos 7 see section below.


### For Centos 7

- On Centos 7 you will need devtoolset-7 , cmake3 and boost

Set up the the compiler environment

```
. .profile.centos7
mkdir -p $ZAPDOS_TEMP
sudo yum install centos-release-scl
sudo yum install devtoolset-7-gcc devtoolset-7-gcc-c++ cmake3 glog-devel libunwind-devel libicu-devel libcurl-devel
# Build boost 1.67 from source
. /opt/rh/devtoolset-7/enable
wget -q -O boost_1_67_0.tar.bz2 "https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.bz2/download"
tar -jxf boost_1_67_0.tar.bz2
cd boost_1_67_0
sh bootstrap.sh --prefix=/opt/local/boost
sudo ./b2 install --prefix=/opt/local/boost
```

Before setting up the thirdparty libraries and compiling this sets the path and alias for cmake

```
. .profile.centos7
```

## Installing thirdparty packages using scripts/thirdparty.sh

Please set the environment ( See .profile.centos7/ubuntu/mac ). There are two environment variables
`ZAPDOS_SOURCE` should be set to the top level directory of the package. 
`ZAPDOS_TPSRC` is where the source code packages are downloaded ( defaults to /opt/backup ).
Order is important since rocksdb needs snappy.

## Compiling

Create a build directory

```
mkdir build
cd build
# Debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
# Release
cmake .. && make
```

On Centos with cmake3 and custom Boost ( See README.LINUX.md )

```
# Debug
cmake3 -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=/opt/local/boost .. && make
# Release
cmake3 -DBOOST_ROOT=/opt/local/boost .. && make
```

Note the number of concurrent connections is limited by number of active threads, you can adjust it
by setting `LIBASYNC_NUM_THREADS` as an environment variable before compiling.

```
export LIBASYNC_NUM_THREADS=30
```

