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
  "payload" : { "keytype" : "K_USERDATA", "name" : "agegroup" , "is_allowed" : true , "is_searchable" : true },
  "payloads" : [
    { "keytype" : "K_LOCALDATA", "name" : "repeatag", "is_allowed" : true, "is_repeated": true, "is_searchable" : true },
    { "keytype" : "K_LOCALDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_WIKIDATA", "name" : "repeatag", "is_allowed" : true, "is_repeated": true, "is_searchable" : true },
    { "keytype" : "K_WIKIDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_USERDATA", "name" : "goodtag", "is_allowed" : true, "is_searchable" : true },
    { "keytype" : "K_EXTERDATA", "name" : "badtag" },
    { "keytype" : "K_USERDATA", "name" : "badtag" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/tagdata/create"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/tagdata/create" <<< "$POST_DATA"
