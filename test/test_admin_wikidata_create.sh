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
    { "unique_id" : "A1001", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Medplus Chemists", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" },
  "payloads" : [
    { "unique_id" : "A1002", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mannada Chemists", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" },
    { "unique_id" : "A1003", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "BandBox Repairs", "summary" : "Description of this shop in Garia on wikipedia", "city" : "Kolkata" },
    { "unique_id" : "M1004", "lang" : "EN", "categories" : [ "wikidata" ],
      "ccode" : "IND", "country" : "India", "importance" : 0.09,
      "title" : "Mistaken Record", "summary" : "Description of this shop in Jadavpur on wikipedia", "city" : "Kolkata" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/wikidata/create"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/wikidata/create" <<< "$POST_DATA"

