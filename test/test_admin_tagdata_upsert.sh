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
  "payload" : { "keytype" : "K_EXTERDATA", "name" : "agegroup" , "is_allowed" : true , "is_searchable" : true },
  "payloads" : [
    { "keytype" : "K_EXTERDATA", "name" : "goodtag", "is_allowed" : true, "is_repeated": true, "is_searchable" : true }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/tagdata/upsert"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/tagdata/upsert" <<< "$POST_DATA"
