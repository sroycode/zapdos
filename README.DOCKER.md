# Running with Docker


The docker containers for nominatim and zapdos can be used thus:

Note: Both the folders contain a run.sh

## Setting up Nominatim

You will need to set these two variables 
- `NHOME` is the directory that will have the osm data , inside it a directory `postgresdata` is created for pgsql.
- `OSMFIL` is the osm dump that resides within it . In the example below it is `india-latest.osm.pbf` 

### Step 1 : create network for docker

```
docker network create my_zapdos
```

### Step 2 : Get Data for Nominatim 


You can also get the other files as required. For example
for India this should do the job

```
wget --output-document=$NHOME/india-latest.osm.pbf http://download.geofabrik.de/asia/india-latest.osm.pbf
wget --output-document=$NHOME/wikipedia_article.sql.bin http://www.nominatim.org/data/wikipedia_article.sql.bin
wget --output-document=$NHOME/wikipedia_redirect.sql.bin http://www.nominatim.org/data/wikipedia_redirect.sql.bin
# wget --output-document=$NHOME/gb_postcode_data.sql.gz http://www.nominatim.org/data/gb_postcode_data.sql.gz
```

### Step 3 : Build Nominatim

TODO: prebuilt

```
cd docker/nominatim
docker build -t nominatim .
```

### Step 4 : Populate Nominatim

To populate the data

```
docker run -v ${NHOME}:/data -t nominatim sh /app/init.sh /data/${OSMFIL} postgresdata 4
```

### Step 5 : Run the Nominatim docker

To run

```
docker run --restart=unless-stopped  -p 6432:5432 -p 7070:8080 \
-d -v ${NHOME}/postgresdata:/var/lib/postgresql/9.5/main 
--network=my_zapdos --name=my_nomina nominatim sh /app/start.sh
```

## Setting up Zapdos

You will need to set these two variables 
- `RHOME` is the directory that will have the zapdos data
- `THOME` is the directory that will have the zapdos dumps which you can delete later , `THOME` is not needed at run time

### Step 1 : build zapdos and make necessary directories

TODO: prebuilt


Note that this `NOMINA_IP` is needed in subsequent commands.


```
docker build -t zapdos .
docker run -v ${RHOME}:/data -t zapdos mkdir -p /data/logs /data/zapdos_logs /data/zapdos_data /data/zapdos_jam /data/zapdos_xap
export NOMINA_IP="`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_nomina`"
```

### Step 2 : Optional : collect data for spellchecker 

This just dumps the names from the nominatim into `${THOME}/EN.txt` .
If you have a different source of data please copy to this file

```
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" -onlyname  ${THOME}/EN.txt 
```

### Step 3 : Optional : load data for spellchecker 

This runs the zapdos service in dryrun mode and just loads the data. Note this is the _only_ case
where THOME gets mounted on zapdos

```
docker run -v ${RHOME}:/data -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_server -config /data/zapdos.conf -jinpath=/data1 -dryrun 
```

### Step 4 : Start the zapdos container

This starts the search container ( as master ) 
The default zapdos.conf should work , however you may want to change the `shared_secret` , note that you can 
also supercede this as a command line parameter ( as done in this example )

```
cp zapdos.conf ${RHOME}/

docker run \
--restart=unless-stopped \
-p 9091:9091 -p 9092:9092 -d \
-v ${RHOME}:/data \
--network=my_zapdos \
--name=my_zapdos \
-d zapdos bash /app/zapdos/run_as_master.sh
```

### Step 5 : collect nominatim data for search 

This just dumps the names from the nominatim into `${THOME}/search_data.txt` .

```
docker run --network=my_zapdos -t zapdos \
zpds_nomina -pgparams "dbname=nominatim user=nominatim host=${NOMINA_IP} port=5432" > ${THOME}/EN_data.txt 
```

### Step 6 : create a user on zapdos and load the data

Find the zapdos IP , start another zapdos docker and use it to load to the already running my_zapdos.
Please change the myuser and mypass to something of your choice.
This will create a new profile and add a search template to it.

```
ZAPDOS_IP=`docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' my_zapdos`
# create user
name=myuser newpass=mypass bash ../../test/test_profile_newprofile.sh
# set search template as user
name=myuser passkey=mypass bash ../../test/test_profile_setsimpletemplate.sh
docker run -v ${THOME}:/data1 --network=my_zapdos \
-t zapdos zpds_addcsv \
-action UPSERT \
-infile /data1/EN_data.txt \
-chunk 10000 -user myuser -passkey mypass \
-update -jurl http://${ZAPDOS_IP}:9091
```
