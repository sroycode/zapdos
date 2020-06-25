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
    { "unique_id" : "A1005", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Bangalore Chemists", "summary" : "Description of this shop in Adugodi on wikipedia", "city" : "Bangalore" },
  "payloads" : [
    { "unique_id" : "M1006", "lang" : "EN", "categories" : [ "osmdata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mistaken Record2", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/wikidata/upsert"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/wikidata/upsert" <<< "$POST_DATA"

