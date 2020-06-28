# Project Zapdos 

[![Build Status](https://travis-ci.org/sroycode/zapdos.svg?branch=master)](https://travis-ci.org/sroycode/zapdos)
[![GitHub (pre-)release](https://img.shields.io/github/release-pre/sroycode/zapdos.svg)](https://github.com/sroycode/zapdos)

This started as a search engine for OSM data. For details see [docs](docs/README.md).

## Code is WIP

This code is work in progress and is to change frequently.

## What is it

1.  As of now this is a completion engine for local data. It uses the `photon` format, so all geocoding libraries that support 
that format should work out-of-the-box.

2.  This is intended to be a drop-in replacement for the project `https://github.com/komoot/photon`.
The endpoint `_query/api/v1/photon/{profile}` is the currently available photon-compatible endpoint. 

3.  The reverse geocoder is with identical syntax at endpoint `_query/api/v1/notoph/{profile}` .

4. Adaptor for importing Nominatim is provided. You can also use your own data. Other adaptors are wip.

5. Profile based search . You can create several profiles , each with different set of rules for the same endpoint. 
There is a `default` profile for all searches which uses a pre-set .

6. Traine-able spellchecker . The spellchecker can be trained from a file containing training data.

7. Supports rudimentary replication. All write queries go to master, read queries are on slave. If master goes down one slave becomes 
master.

8. There is now support for a separate search store for text data. Basic idea is to have local data queries which will mostly be
location based in one index and wiki data which will mostly be text based in another. This is with identical syntax as above photon
at endpoint `_query/api/v1/textdata/{profile}` .

## Compiling

For instructions on setting up the development environment see [docs/COMPILE.md](docs/COMPILE.md)

## Testing

Look at the folder test for expected query.
The test/test_ scripts are for testing, you will need httpie.
For details see [docs](docs/README.md)

## Increasing concurrency

Note the number of concurrent connections is limited by number of active threads, you can adjust it
by setting `LIBASYNC_NUM_THREADS` as an environment variable before compiling. By default this will 
take the no of CPU cores of the server.

```
export LIBASYNC_NUM_THREADS=32
```

## Acknowledgements

This project uses code from

- https://github.com/Amanieu/asyncplusplus
- https://github.com/greg7mdp/sparsepp

- https://github.com/eidheim/Simple-Web-Server ( web server )
- https://github.com/yinqiwen/ardb ( Geohash )
- https://github.com/bakwc/JamSpell ( Spell checker )

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
