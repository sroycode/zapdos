# Compile from Source

## Creating Development Environment

You need to install these tools using standard utils.
- cmake 3+,
- boost 1.62+
- clang++/gcc5 ( supporting C++11)
- icu4c
- glog and gflags.
- libcurl 7.0+ 
- httpie for testing
- openssl 1.0+
- gperftools 2.5
- protobuf 3.5.0
- snappy 1.1.7
- rocksdb 5.6
- crypto++ 8.0
- xapian 1.4+

Optional
- ctemplate

On mac +homebrew and ubuntu-18 +apt default versions work fine.

### For Mac +homebrew

```
brew install glog gflags gperftools libicu icu4c curl-openssl tbb rocksdb protobuf gperf snappy cryptopp xapian
# optional ctemplate
brew install ctemplate
```

### For Ubuntu 18

```
sudo add-apt-repository -y ppa:exonum/rocksdb
sudo add-apt-repository -y ppa:maarten-fonville/protobuf
sudo apt update -y
sudo apt install -y \
	wget ccache cmake make libtool pkg-config g++ gcc autoconf automake curl jq lcov \
	protobuf-compiler vim-common libboost-all-dev libboost-all-dev libcurl4-openssl-dev zlib1g-dev liblz4-dev libprotobuf-dev \
	libgoogle-glog-dev libgflags-dev libgoogle-perftools-dev libsnappy-dev libbz2-dev libz-dev \
	libtbb-dev libzstd-dev librocksdb-dev libcrypto++-dev
# xapian
sudo add-apt-repository -y ppa:xapian-backports/ppa
sudo apt update -y
sudo apt install -y libxapian-dev 
# optional ctemplate
sudo apt install -y libctemplate-dev

```

## Parameters

| CMake Param | Description |
| ---- | ----------- |
| `XSTR_BUILD_WITH_ROCKSDB` | build using Facebook RocksDB is default |
| `XSTR_BUILD_WITH_LEVELDB` | build using Google LevelDB , not tested |
| `XSTR_BUILD_WITH_XAPIAN` | build Xapian ie Search , this is kept for developers who may want to use this as a framework |
| `XSTR_BUILD_WITH_CTEMPLATE` | HTML support, not needed |
| `XSTR_USE_SEPARATE_LOGDB` | Use separate database for logdb , in large installations with frequent updates |
| `XSTR_USE_SPARSE_HASH_CACHE` | Use sparse hash instead of unordered_map for cache, slower but takes less memory  |


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

Notes:

With custom Boost add `-DBOOST_ROOT=/opt/local/boost`

## Running

```
export GLOG_logtostderr=1  # to log to stderr
./zpds_server --config ../etc/zapdos.conf
```

## Testing

Look at the folder test for expected query.
The test/test_ scripts are for testing, you will need `httpie` and `jq`.

## Increasing concurrency at runtime

Note the number of concurrent connections is limited by number of active threads, you can adjust it
by setting `LIBASYNC_NUM_THREADS` as an environment variable.
By default this will take 4 or the no of CPU cores of the server if more.

```
export LIBASYNC_NUM_THREADS=30
```
