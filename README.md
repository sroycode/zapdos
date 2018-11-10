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

## Compiling from source

For instructions see README.COMPILE.md

## Running

1. You need to have a config file , see README.CONFIG.md for details. There is a sample in etc for you to edit

2. If you want to enable spellcheck , add a file for training in `jinpath` as EN.txt 

3. Start the server ( default starts in master mode) . Default config listens 9091 for service + 9092 for intermachine

```
./zpds_server --config ../etc/zpds.conf
```

3. To start another server in slave mode (optional) pointing to master intermachine port started at port 9092 of same machine

```
./zpds_server --config ../etc/zpds2.conf --master="http://127.0.0.1:9092"
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
