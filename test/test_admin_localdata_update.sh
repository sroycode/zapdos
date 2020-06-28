#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
if [ -z "${sessionkey}" ] ; then echo "define sessionkey by logging in"; exit 1; fi
username=${username:="admin"}

POST_DATA='{
  "sessionkey" : "'${sessionkey}'",
  "username" : "'${username}'",
  "payload" : 
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "lon" : 88.88888, "lat" : 22.22222,
      "fld_name" : "Mannada Chemists Updated", "fld_area" : "Old Jadavpur", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "lon" : 88.88888, "lat" : 22.22222,
      "fld_name" : "BandBox Repairs Updated", "fld_area" : "Old Garia", "city" : "Kolkata" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/localdata/update"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/localdata/update" <<< "$POST_DATA"
