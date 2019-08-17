# Create Search Data

## 

RHOME=/Users/shreos/Docker/zapdos
THOME=/Users/shreos/Docker/zapdos_data
mkdir -p ${RHOME} ${THOME}
NOMINA_IP="`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_nomina`"
SHARED_SECRET="PLEASE_REPLACE_THIS_BY_YOUR_SHARED_SECRET"

function build () {
docker build -t zapdos . \
&& docker run -v ${RHOME}:/data --network=my_zapdos -t zapdos mkdir -p \
/data/logs /data/zapdos_logs /data/zapdos_data /data/zapdos_jam /data/zapdos_xap /data/zapdos_jin
}

function make_spell_data () {
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" -onlyname > ${THOME}/EN.txt 
}

function make_srch_data () {
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" > ${THOME}/EN_data.txt 
}

function load_spell_data () {
docker run -v ${RHOME}:/data -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_server -config=/usr/local/etc/zapdos.conf -jinpath=/data1 -dryrun 
}

function run () {
docker run \
--restart=unless-stopped \
-p 9091:9091 -p 9092:9092 -d \
-v ${RHOME}:/data \
--network=my_zapdos \
--name=my_zapdos \
-d zapdos bash /app/zapdos/run_as_master.sh
}

function load_srch_data () {
ZAPDOS_IP=`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_zapdos`
export TESTURL="http://localhost:9091"
name=myuser newpass=mypass bash ../../test/test_profile_newprofile.sh
name=myuser passkey=mypass bash ../../test/test_profile_setsimpletemplate.sh
docker run -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_addlocal \
-action UPSERT \
-infile /data1/EN_data.txt \
-chunk 5000 -user myuser -passkey mypass -update -jurl http://${ZAPDOS_IP}:9091
}

if [ $# -ne 1 ] ; then echo "Usage $0 <build|makes|loads|run|maked|loadd>" ; exit 1 ; fi

if [ "$1" = "build" ] ; then build ; fi
if [ "$1" = "makes" ] ; then make_spell_data ; fi
if [ "$1" = "loads" ] ; then load_spell_data ; fi
if [ "$1" = "run" ] ; then run ; fi
if [ "$1" = "maked" ] ; then make_srch_data ; fi
if [ "$1" = "loadd" ] ; then load_srch_data ; fi