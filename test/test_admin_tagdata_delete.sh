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
  "payload" : { "keytype" : "K_EXTERDATA", "name" : "badtag" },
  "payloads" : [
    { "keytype" : "K_USERDATA", "name" : "badtag"  }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/tagdata/delete"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/tagdata/delete" <<< "$POST_DATA"
