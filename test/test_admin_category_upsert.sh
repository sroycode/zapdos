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
  "payload" : { "name" : "someother" , "description" : "example of upsert"},
  "payloads" : [
    { "name" : "inhouse" , "description" : "inhouse production upsert" },
    { "name" : "mistake2", "description" : "to demonstrate delete" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/category/upsert"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/category/upsert" <<< "$POST_DATA"
