#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
if [ -z "${sessionkey}" ] ; then echo "define sessionkey by logging in"; exit 1; fi
username=${username:="user001"}

POST_DATA='{
  "sessionkey" : "'${sessionkey}'",
  "username" : "'${username}'",
  "payload" : { "primary_email": "test@best11.com" }
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_user/api/v1/userdata/update_email"
${HTTPIE} POST "${TESTURL}/_user/api/v1/userdata/update_email" <<< "$POST_DATA"
