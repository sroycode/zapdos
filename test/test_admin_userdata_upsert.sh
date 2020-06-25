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
  "payload" : { "name" : "user001", "passkey" : "user001p1" , "given_name" : "atest 001" , "description" : "added later" },
  "payloads" : [
     { "name" : "user003", "passkey" : "user003p1" , "given_name" : "test 003" , "tags" : [ { "name" : "goodtag" , "value" : "formal" } ] }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/userdata/upsert"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/userdata/upsert" <<< "$POST_DATA"
