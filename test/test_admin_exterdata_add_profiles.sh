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
  "payload" : { "name" : "test001",
    "profiles" : [
    {
      "query_type" : "QRY_COMPLETION_PHOTON",
      "rules" : [
        {
          "weight" : 100, "max_records" : 10, "oneword_length" : 2,
          "desc" : "one word query for upto 2 chars",
          "limit_type" : "L_NONE", "search_type" : "S_BEGINWITH", "order_type" : "O_DEFAULT", "input_type" : "I_ONEWORD"
        },
        {
          "weight" : 90, "max_records" : 10,
          "desc" : "by distance 1km bands",
          "distance_band" : 1000,
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DIST_BAND", "input_type" : "I_DEFAULT"
        },
        {
          "weight" : 80, "max_records" : 10,
          "desc" : "if not local",
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        }
      ]
    },
    {
      "query_type" : "QRY_COMPLETION_NOTOPH",
      "rules" : [
        {
          "weight" : 100, "max_records" : 5,
          "desc" : "by distance only so band is small",
          "distance_band" : 10,
          "limit_type" : "L_NBRHASH5", "search_type" : "S_DEFAULT", "order_type" : "O_DIST_BAND", "input_type" : "I_DEFAULT"
        }
      ]
    },
    {
      "query_type" : "QRY_COMPLETION_WIKIDATA",
      "rules" : [
        {
          "weight" : 100, "max_records" : 10, "oneword_length" : 2,
          "desc" : "one word query for upto 2 chars",
          "limit_type" : "L_NONE", "search_type" : "S_BEGINWITH", "order_type" : "O_DEFAULT", "input_type" : "I_ONEWORD"
        },
        {
          "weight" : 90, "max_records" : 10,
          "desc" : "find in name",
          "limit_type" : "L_NONE", "search_type" : "S_NAME", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        },
        {
          "weight" : 80, "max_records" : 10,
          "desc" : "if not in name",
          "limit_type" : "L_NONE", "search_type" : "S_DEFAULT", "order_type" : "O_DEFAULT", "input_type" : "I_DEFAULT"
        }
      ]
    }
    ]
  } 
}'

## ---- main

# wget -q -O - --header="Content-Type: application/json" --post-data="${POST_DATA}" "${TESTURL}/_admin/api/v1/exterdata/add_profiles"
${HTTPIE} POST "${TESTURL}/_admin/api/v1/exterdata/add_profiles" <<< "$POST_DATA"
