#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export name=${name:="shreos"}
export passkey=${passkey:="shreos123"}
export POST_DATA='{
   "passkey" : "'${passkey}'",
   "name" : "'${name}'",
   "records" : [
      { "styp" : "S_TEXT", "lang" : "EN", "uniqueid" : "895" , "scode": "WB"},
      { "styp" : "S_TEXT", "lang" : "EN", "uniqueid" : "1005" , "scode": "WB"}
   ]
}'
## ---- main

wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_data/api/v1/textrecord/merge"
