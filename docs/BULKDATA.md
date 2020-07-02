# Populating Data

For osm data, we prefer to use Nominatim. If you want to have your customized data, 
Bulk data is available for osm in 

## Sample Data

Sample data is available in test folder. After starting the server, use the admin login to populate data.
Note that if you have changed the shared_secret or adminpass you want to set that.

```
export ZAPDOS_HOME=/path/to/zapdos/source
export shared_secret=YOUR-CHANGED_SHARED_SECRET
export default_adminpass=YOUR_CHANGED-ADMINPASS
bash ${ZAPDOS_HOME}/test/export_admin_login.sh > .login
# check if the login  is valid
cat .login
# should be something like `export sessionkey="0jAHkFNvIEbmOesy7vZWlFpxGBW6b7IMQC5NvQ=="`
```

Populate the test data

```
. .login
zpds_adddata -dtype localjson -infile ${ZAPDOS_HOME}/test/photon.json -chunk 1000 -username admin -sessionkey $sessionkey -jurl http://localhost:9093 -update
zpds_adddata -dtype wikijson -infile ${ZAPDOS_HOME}/test/wikidata.json -chunk 1000 -username admin -sessionkey $sessionkey -jurl http://localhost:9093 -update
```

Test the search is working

```
q="kor" bash ${ZAPDOS_HOME}/test/test_query_completion_photon.sh
bash ${ZAPDOS_HOME}/test/test_query_completion_notoph.sh
q="sat" bash ${ZAPDOS_HOME}/test/test_query_completion_textdata.sh
```

## Populating several hundred million records

This is common to the two examples below.

For the first time dump, it is better to build the search index separately.
Adding the initial data otherwise takes days. 
Zapdos uses xapian under the hood, see [xapian reference](https://xapian.org/docs/admin_notes.html#merging-databases).

There is an utility `zpds_xapindex` that can do this.

Note that the `dbpath` is to be set to the same as in config, so for the example we assume

```
[work]
datadir = /home/ubuntu/tmp/data/zapdos/_data
...
[xapian]
datadir = /home/ubuntu/tmp/data/zapdos/_xap
```

Steps required:

1. Start the database and set a flag for it not to index the data. Effect of this is session only,
when you restart `zpds_server` later on it will index at run time again.

2. Now shutdown the zpds_server normally. Then generate the index in a temporary folder

3. Copy the index to the server xapian index directory , usually the name format is `{LANG}_{INDEXTYPE}` , like EN_I_LOCALDATA.

## Populating entire Planet

### Getting the data

OSM dumps are available in photon format, thanks to Komoot and GraphHopper.
You can download the 24-Jun-2020 dump from vnaad requester-paid S3 bucket. This was made from a photon dump (see below).
The zipped file is about 12 GB 

```
aws s3 cp --request-payer s3://vnaad-zapdos-data/20200624/photon.json.gz . 
```

### Add the data to zapdos

Note that the `dbpath` is to be set to the same as in config, same as above.

Start the database and set a flag for it not to index the data.
Effect of this is session only , when you restart `zpds_server` later on it will index at run time again.

```
export shared_secret=YOUR-CHANGED_SHARED_SECRET
bash ${ZAPDOS_HOME}/test/test_command_no_xapian.sh
. .login
# this populates the data dump without xapian index
zpds_adddata -dtype localjson -infile photon.json -chunk 5000 -username admin -sessionkey $sessionkey -jurl http://localhost:9093 -update
```

Now shutdown the zpds_server normally. Then generate the index in a temporary folder , in this example `testme`.

```
# Takes about 23 hours on a r5.2xlarge instance, less on c4/5
zpds_xapindex -threads 7 -indextype "localdata" -dbpath=/home/ubuntu/tmp/data/zapdos/_data --xapath `pwd`/testme --nomerge
# Takes about 4 hours on a r5.2xlarge instance, less on c4/5
zpds_xapindex -threads 7 -indextype "localdata" -dbpath=/home/ubuntu/tmp/data/zapdos/_data --xapath `pwd`/testme --noindex
# Copy the data to xapdos xapian index
cp `pwd`/testme/main/EN_I_LOCALDATA/*.* /home/ubuntu/tmp/data/zapdos/_xap/EN_I_LOCALDATA/
```

Start zapdos server. You are ready.

### Reference Only : How the photon.json was created

This generates the `photon.json` used above. Note however, this will require about 700 GB free space.
The dump at Graphhopper is in elasticsearch format, so we load it through photon and extract by elasticdump.

```
wget -O - http://download1.graphhopper.com/public/photon-db-latest.tar.bz2 | lbzip2 -cd | tar x
# Download and run photon, this exposes ES to port 9200
wget https://github.com/komoot/photon/releases/download/0.3.2/photon-0.3.2.jar
java -jar photon-0.3.2.jar
# Use Elasticdump to extract
npm install elasticdump
export PATH=$PATH:node_modules/.bin
elasticdump --limit=5000 --concurrency=5 --input=http://127.0.0.1:9200/photon --output=photon.json --type=data
```

## Populating MediaWiki

### Getting the data

The zipped file is about 78 GB , unzips to over 1.1 TB , so keep 1.5 TB space handy.
The default `zpds_extractwiki` rejects all records without description field, so the extract is small.
Modify the `src/tools/ExtractWiki.cc` to suite your needs. Contact us if you need to extract targeted data from this.
You can delete the unzipped `latest-all.json` once the first phase is done

```
wget https://dumps.wikimedia.org/wikidatawiki/entities/latest-all.json.gz
pigz -kd latest-all.json.gz
zpds_extractwiki latest-all.json > wikidata.json
rm -f latest-all.json
```

### Add the data to zapdos

Start the database and set a flag for it not to index the data.

```
export shared_secret=YOUR-CHANGED_SHARED_SECRET
bash ${ZAPDOS_HOME}/test/test_command_no_xapian.sh
. .login
# this populates the data dump without xapian index
zpds_adddata -dtype wikijson -infile wikidata.json -chunk 5000 -username admin -sessionkey $sessionkey -jurl http://localhost:9093 -update
```

Now shutdown the zpds_server normally. Then generate the index in a temporary folder , in this example `testme1`.

```
zpds_xapindex -threads 7 -indextype "wikidata" -dbpath=/home/ubuntu/tmp/data/zapdos/_data --xapath `pwd`/testme1 --nomerge
zpds_xapindex -threads 7 -indextype "wikidata" -dbpath=/home/ubuntu/tmp/data/zapdos/_data --xapath `pwd`/testme1 --noindex
# Copy the data to xapdos xapian index
cp `pwd`/testme1/main/EN_I_WIKIDATA/*.* /home/ubuntu/tmp/data/zapdos/_xap/EN_I_WIKIDATA/
```

Start zapdos server. You are ready.
