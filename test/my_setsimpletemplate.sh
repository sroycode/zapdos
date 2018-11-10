#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"}

## ---- functions

function set_local_oneword () {
local qtyp="QRY_COMPLETION_LOCAL_ONEWORD";
local qprof='{"orders" : [
  {
   "rec_tagid": "#t202", "desc": "areasensitive in geohash", "weight": "100",
   "rec_rtyp": "R_BUSINESS", "rec_count": "5",
   "limit_type": "L_GEOHASH", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t222", "desc": "max category", "weight": "100",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10",
   "limit_type": "L_MAX_CCODE", "search_type": "S_FIRSTWORD", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t232", "desc": "all business in city", "weight": "90",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10",
   "limit_type": "L_TOP_CITY", "search_type": "S_FIRSTWORD", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t242", "desc": "category", "weight": "90",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10",
   "limit_type": "L_MAX_CCODE", "search_type": "S_FIRSTWORD", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t252", "desc": "all business", "weight": "80",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10",
   "limit_type": "L_TOP_CCODE", "search_type": "S_FIRSTWORD", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  }
]}';

qtyp="${qtyp}" qprof="${qprof}" bash ${SRCDIR}/test_profile_setsimpletemplate.sh
}



function set_local_notsplit () {
local qtyp="QRY_COMPLETION_LOCAL_NOTSPLIT";
local qprof='{"orders" : [
  {
   "rec_tagid": "#t202", "desc": "areasensitive in geohash", "weight": "100",
   "rec_rtyp": "R_BUSINESS", "rec_count": "5", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_GEOHASH", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t212", "desc": "top business in city", "weight": "90",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_TOP_CITY", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t222", "desc": "all business in city", "weight": "80",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_CITY", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t232", "desc": "top category", "weight": "80",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_TOP_CCODE", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t242", "desc": "all business", "weight": "70",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t252", "desc": "all category", "weight": "60",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t262", "desc": "partial top business in city", "weight": "50",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_TOP_CITY", "search_type": "S_PARTIAL_BEGIN", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  }
]}';

qtyp="${qtyp}" qprof="${qprof}" bash ${SRCDIR}/test_profile_setsimpletemplate.sh
}


function set_local_split () {
local qtyp="QRY_COMPLETION_LOCAL_SPLIT";
local qprof='{"orders" : [
  {
   "rec_tagid": "#t202", "desc": "single brand entry in city", "weight": "100",
   "rec_rtyp": "R_BRAND", "rec_count":"2",
   "limit_type": "L_CITY", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_BRAND_CITY"
  },
  {
   "rec_tagid": "#t212", "desc": "brand entries in city", "weight": "80",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_BRAND_CITY"
  },
  {
   "rec_tagid": "#t222", "desc": "areasensitive in geohash", "weight": "90",
   "rec_rtyp": "R_BUSINESS", "rec_count": "5", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_GEOHASH", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t232", "desc": "top business in city", "weight": "80",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_TOP_CITY", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t242", "desc": "all business in city", "weight": "70",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_CITY", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t252", "desc": "all business", "weight": "60",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_QUERY"
  },
  {
   "rec_tagid": "#t292", "desc": "locations", "weight": "0",
   "rec_rtyp": "R_LOCATION", "rec_count": "10", "distance_def": "6000", "distance_band": "3000",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_WHERE"
  },
  {
   "rec_tagid": "#t262", "desc": "top category + loc", "weight": "70",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10", "distance_def": "6000", "distance_band": "3000",
   "mrg_tagid": "#t292", "mrg_rtyp": "R_LOCATION", "mrg_count" : "10",
   "limit_type": "L_TOP_CCODE", "search_type": "S_DEFAULT", "order_type": "O_DIST_CALL_BAND", "input_type": "I_WHAT"
  },
  {
   "rec_tagid": "#t272", "desc": "all category + loc", "weight": "60",
   "rec_rtyp": "R_CATEGORY", "rec_count": "10",
   "mrg_tagid": "#t292", "mrg_rtyp": "R_LOCATION", "mrg_count" : "5",
   "limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT", "input_type": "I_WHAT"
  },
  {
   "rec_tagid": "#t282", "desc": "partial top business in city", "weight": "50",
   "rec_rtyp": "R_BUSINESS", "rec_count": "10", "distance_def": "6000", "distance_band": "1000",
   "limit_type": "L_TOP_CITY", "search_type": "S_PARTIAL_BEGIN", "order_type": "O_DIST_CALL_BAND", "input_type": "I_QUERY"
  }
]}';

qtyp="${qtyp}" qprof="${qprof}" bash ${SRCDIR}/test_profile_setsimpletemplate.sh
}

## ---- main

set_local_oneword
set_local_notsplit
set_local_split
