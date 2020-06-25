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
  "payload" : { "name" : "user001", "passkey" : "user001p1" , "given_name" : "user 001" },
  "payloads" : [
     { "name" : "user002", "passkey" : "user002p1" , "given_name" : "user 002" }
  ]
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/userdata/create"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/userdata/create" <<< "$POST_DATA"
