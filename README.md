# Project Zapdos

Local Search with OSM data

## Code is WIP

This code is work in progress and is to change frequently.

## What is it

This is a search engine that sets up a local search from OSM data

Supports rudimentary replication. All write queries go to master, read queries are on slave.

For replication , the service listens to two ports , you can expose the main port to public, the second port is for
communication between servers.

## Run using Docker

The easiest way to take it for a spin is by using docker , follow the instructions in README.DOCKER.md


## Compiling

For instructions on setting up the development environment see README.COMPILE.md

Create a build directory

```
mkdir build
cd build
# Debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
# Release
cmake .. && make
```

On Centos with cmake3 and custom Boost

```
# Debug
cmake3 -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=/opt/local/boost .. && make
# Release
cmake3 -DBOOST_ROOT=/opt/local/boost .. && make
```

## Running

1. You need to have a config file , see README.CONFIG.md for details. There is a sample in etc for you to edit

2. If you want to enable spellcheck , add a file for training in `jinpath` as EN.txt . The jinpath can be set in config and 
can be overridden at command line. Note this is needed only once so subsequent restarts will not need this.

3. Start the server ( default starts in master mode) . Default config listens 9091 for service + 9092 for intermachine

```
./zpds_server --config ../etc/zpds.conf -jinpath=/path/to/directory
# Subsequent restarts omit the jinpath
```

3. To start another server in slave mode (optional) pointing to master started at port 9092 of same machine. You will

```
./zpds_server --config ../etc/zpds2.conf --master="http://127.0.0.1:9092" -jinpath=/path/to/directory
```

## Testing

Look at the folder test for expected query.
The test/test_ scripts are for testing, you will need httpie.

## Increasing concurrency

Note the number of concurrent connections is limited by number of active threads, you can adjust it
by setting `LIBASYNC_NUM_THREADS` as an environment variable before compiling. By default this will 
take the no of CPU cores of the server.

```
export LIBASYNC_NUM_THREADS=30
```

## Acknowledgements

This project uses code from

- https://github.com/Tencent/rapidjson
- https://github.com/Amanieu/asyncplusplus
- https://github.com/whoshuu/cpr
- https://github.com/greg7mdp/sparsepp

- https://github.com/eidheim/Simple-Web-Server ( web server )
- https://github.com/yinqiwen/ardb ( Geohash )
- https://github.com/bakwc/JamSpell ( Spell checker )

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
