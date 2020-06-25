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
  "payloads" : [
    { "keytype" : "K_LOCALDATA", "name" : "goodtag" },
    { "keytype" : "K_LOCALDATA", "name" : "repeatag" },
    { "keytype" : "K_USERDATA", "name" : "badtag" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/tagdata/getmany"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/tagdata/getmany" <<< "$POST_DATA"
