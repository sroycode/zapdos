#!/bin/bash

RHOME=/Users/shreos/Docker/zapdos
THOME=/Users/shreos/Docker/zapdos_data
mkdir -p ${RHOME} ${THOME}
NOMINA_IP="`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_nomina`"
SHARED_SECRET="PLEASE_REPLACE_THIS_BY_YOUR_SHARED_SECRET"

function build () {
docker build -t zapdos . \
&& cp zapdos.conf ${RHOME}/zapdos.conf \
&& docker run -v ${RHOME}:/data --network=my_zapdos -t zapdos mkdir -p \
/data/logs /data/zapdos_logs /data/zapdos_data /data/zapdos_jam /data/zapdos_xap /data/zapdos_jin
}

function make_spell_data () {
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" \
-onlyname -range "1-999999999" > ${THOME}/EN.txt 
}

function make_srch_data () {
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" \
-range "1-999999999" > ${THOME}/data_search.txt 
}

function load_spell_data () {
docker run -v ${RHOME}:/data -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_server -config /data/zapdos.conf -jinpath=/data1 -dryrun 
}

function run () {
docker run \
--restart=unless-stopped \
-p 9091:9091 -p 9092:9092 -d \
-v ${RHOME}:/data \
--network=my_zapdos \
--name=my_zapdos \
-d zapdos zpds_server -config /data/zapdos.conf -shared_secret "${SHARED_SECRET}"
}

function load_srch_data () {
ZAPDOS_IP=`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_zapdos`
name=myuser newpass=mypass bash ../../test/test_profile_newprofile.sh
shared_secret="${SHARED_SECRET}" name=myuser newpass=mypass bash ../../test/test_profile_newprofile.sh
docker run -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_addcsv \
-action UPSERT \
-infile /data1/tmp/data_search.txt \
-chunk 10000 -user myuser -passkey mypass -print 
# -update -jurl http://${ZAPDOS_IP}:9091
}

if [ $# -ne 1 ] ; then echo "Usage $0 <build|makes|loads|run|maked|loadd>" ; exit 1 ; fi

if [ "$1" = "build" ] ; then build ; fi
if [ "$1" = "makes" ] ; then make_spell_data ; fi
if [ "$1" = "loads" ] ; then load_spell_data ; fi
if [ "$1" = "run" ] ; then run ; fi
if [ "$1" = "maked" ] ; then make_srch_data ; fi
if [ "$1" = "loadd" ] ; then load_srch_data ; fi
