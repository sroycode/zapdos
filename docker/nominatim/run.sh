#!/bin/bash

if [ -z "$NHOME" ] ; then echo "Please set NHOME for nominatim data" ; exit 1; fi
if [ -z "$OSMFIL" ] ; then echo "Please set OSMFIL osm file" ; exit 1; fi

function build() {
docker network create my_zapdos
docker build -t nominatim .
docker run -v ${NHOME}:/data -t nominatim sh /app/init.sh /data/${OSMFIL} postgresdata 4
}

function run () {
docker run \
--restart=unless-stopped \
-p 6432:5432 -p 7070:8080 \
-v ${NHOME}/postgresdata:/var/lib/postgresql/9.5/main \
--network=my_zapdos \
--name=my_nomina \
-d nominatim sh /app/start.sh
}

if [ $# -ne 1 ] ; then echo "Usage $0 <build|run>" ; exit 1 ; fi

if [ "$1" = "build" ] ; then build ; fi
if [ "$1" = "run" ] ; then run ; fi
