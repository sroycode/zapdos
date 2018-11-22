# Project Zapdos 

[![Build Status](https://travis-ci.org/sroycode/zapdos.svg?branch=master)](https://travis-ci.org/sroycode/zapdos)
[![GitHub (pre-)release](https://img.shields.io/github/release-pre/sroycode/zapdos.svg)](https://github.com/sroycode/zapdos)

Local Search with OSM data

## Code is WIP

This code is work in progress and is to change frequently.

* Data structrures have changed, so if you upgrade from 0.4.1 to 0.5.0 or higher you need to dump the data again

## What is it

1.  As of now this is a completion engine for local or data.

2.  This is intended to be a drop-in replacement for the project `https://github.com/komoot/photon`.
The endpoint `_query/api/v1/photon/{profile}` is the currently available photon-compatible endpoint. 

3.  The reverse geocoder is with identical syntax at endpoint `_query/api/v1/notoph/{profile}` .

4. Adaptor for importing Nominatim is provided. You can also use your own data. Other adaptors are wip.

5. Profile based search . You can create several profiles , each with different set of rules for the same endpoint. 

6. Traine-able spellchecker . The spellchecker can be trained from a file containing training data.

7. Supports rudimentary replication. All write queries go to master, read queries are on slave. If master goes down one slave becomes 
master. See README.REPLICATION.md for details.

8. There is now support for a separate search store for text data. Basic idea is to have local data queries which will mostly be
location based in one index and wiki data which will mostly be text based in another. This is with identical syntax as above photon
at endpoint `_query/api/v1/textdata/{profile}` .

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

This example below assumes two variables are set :
- `RHOME` is the directory that will have the zapdos data
- `THOME` is the directory that will have the zapdos dumps which you can delete later , `THOME` is not needed at run time

1. You need to have a config file , see README.CONFIG.md for details. There is a sample in etc for you to edit

2. Getting the data: Assuming you have Nominatim Docker setup and its pgsql is exposed to 6432 ( see README.DOCKER.md ),
this will dump the names from the nominatim into `${THOME}/EN.txt` and the data in `${THOME}/EN_data.txt` .
Most users will create their own spell files.

```
# only name for spelling
./zpds_nomina -pgparams "dbname=nominatim user=nominatim host=localhost port=6432" -onlyname  > ${THOME}/EN.txt 
# actual data
./zpds_nomina -pgparams "dbname=nominatim user=nominatim host=localhost port=6432" > ${THOME}/EN_data.txt 
```

2. If you want to enable spellcheck , add a file for training in `jinpath` as EN.txt . The jinpath can be set in config and 
can be overridden at command line. Note this is needed only once so subsequent restarts will not need this.

3. Start the server ( default starts in master mode) . Default config listens 9091 for service + 9092 for intermachine.
See step below for creating the spellcheck training file.

```
export GLOG_logtostderr=1  # to log to stderr
./zpds_server --config ../etc/zpds.conf -jinpath=${THOME}
# Subsequent restarts omit the jinpath
```

4. Add the data 

```
# creates user
shared_secret="${SHARED_SECRET}" name=myuser newpass=mypass bash ../../test/test_profile_newprofile.sh
# loads data
name=myuser passkey=mypass bash ../../test/test_profile_setsimpletemplate.sh
# creates search profile for user
zpds_addlocal -action UPSERT -infile ${THOME}/EN_data.txt -user myuser -passkey mypass -update -jurl http://localhost:9091
```

5. Query

Have a look at `test/test_completion_photon.sh`


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
