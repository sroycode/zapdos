# Running with Docker

```
docker build -t zapdos .
docker network create my_zapdos
docker run -t zapdos mkdir -p /data1/zapdos/logs
```

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
-t zapdos zpds_addlocal \
-action UPSERT \
-infile /data1/EN_data.txt \
-chunk 10000 -user myuser -passkey mypass \
-update -jurl http://${ZAPDOS_IP}:9091
```
