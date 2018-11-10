#!/bin/bash
export RIBHU_TDATA=/data/input
mkdir -p ${RIBHU_TDATA}
export GLOG_logtostderr=1
zpds_nomina \
-pgparams "dbname=nominatim user=nominatim host=$1 port=5432" \
-onlyname \
-range "1-999999999" > ${RIBHU_TDATA}/data_spell.txt \
&& zpds_nomina \
-pgparams "dbname=nominatim user=nominatim host=$1 port=5432" \
-range "1-999999999" > ${RIBHU_TDATA}/data_main.txt
