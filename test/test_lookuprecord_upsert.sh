#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export name=${name:="shreos"}
export passkey=${passkey:="shreos123"}
export POST_DATA='{
   "passkey" : "'${passkey}'",
   "name" : "'${name}'",
   "records" : [
      {
         "lang" : "EN",
         "ccode" : "IND",
         "country" : "India",
         "styp" : "S_OSM",
         "importance" : 0.09,
         "uniqueid" : "895",
         "fld_name" : "Medplus Chemists",
         "fld_area" : "Jadavpur",
         "city" : "Kolkata"
      },
      {
         "lang" : "EN",
         "ccode" : "IND",
         "country" : "India",
         "styp" : "S_OSM",
         "importance" : 0.10,
         "uniqueid" : "1005",
         "fld_name" : "Bedwin Restaurant",
         "fld_area" : "Jadavpur",
         "city" : "Kolkata"
      }
   ]
}'
## ---- main

wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_data/api/v1/lookuprecord/upsert"
