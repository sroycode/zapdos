#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export POST_DATA='{
   "records" : [
      { "styp" : "S_OSM", "lang" : "EN", "uniqueid" : "895" },
      { "styp" : "S_OSM", "lang" : "EN", "uniqueid" : "1005" }
   ]
}'
## ---- main

wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_data/api/v1/lookuprecord/getnd"
