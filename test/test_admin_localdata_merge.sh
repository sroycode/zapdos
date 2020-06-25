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
    { "unique_id" : "A1001", "importance" : 0.29, "tags" : [ { "name" : "repeatag" , "value" : "yet another" } ] },
  "payloads" : [
    { "unique_id" : "A1002", "importance" : 0.39, "fld_name" : "something" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/localdata/merge"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/localdata/merge" <<< "$POST_DATA"

