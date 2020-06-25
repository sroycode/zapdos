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
  "payload" : { "name" : "inhouse" , "description" : "inhouse production" },
  "payloads" : [
    { "name" : "osmdata", "description" : "default osm data" },
    { "name" : "wikidata", "description" : "default wiki data" },
    { "name" : "alwaysfree", "description" : "always free host bundle" },
    { "name" : "paiddata", "description" : "example of paid data" , "cansee_type" : "C_PAID" , "cansee_price" : 100 },
    { "name" : "mistake1", "description" : "to demonstrate delete" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/category/create"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/category/create" <<< "$POST_DATA"
