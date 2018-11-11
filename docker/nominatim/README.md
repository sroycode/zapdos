# Nominatim Docker (Nominatim version 3.1)

1. Build
  ```
  docker build -t nominatim .
  ```
2. Copy <your_country>.osm.pbf to a local directory (i.e. /home/me/nominatimdata)

3. Initialize Nominatim Database
  ```
  docker run -t -v ${HOME}/Docker/nomina:/data nominatim  sh /app/init.sh /data/bangalore-20181026.osm.pbf postgresdata 4
  ```
  Where 4 is the number of threads to use during import. In general the import of data in postgres is a very time consuming
  process that may take hours or days. If you run this process on a multiprocessor system make sure that it makes the best use
  of it. You can delete the /home/me/nominatimdata/merged.osm.pbf once the import is finished.


4. After the import is finished the /home/me/nominatimdata/postgresdata folder will contain the full postgress binaries of
   a postgis/nominatim database. The easiest way to start the nominatim as a single node is the following:
   ```
   docker run --restart=always -p 6432:5432 -p 7070:8080 -d -v ${HOME}/Docker/nomina/postgresdata:/var/lib/postgresql/9.5/main nominatim sh /app/start.sh
   ```
