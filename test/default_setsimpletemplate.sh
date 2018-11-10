#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"}
export qprof='{}';
export ignore=true;


for qtyp in \
	QRY_COMPLETION_LOCAL_ONEWORD \
	QRY_COMPLETION_LOCAL_SPLIT \
	QRY_COMPLETION_LOCAL_NOTSPLIT \
	QRY_COMPLETION_LOCAL_COMMON_BUSINESS \
	QRY_COMPLETION_LOCAL_COMMON_CATEGORY \
	QRY_COMPLETION_LOCAL_COMMON_BRAND \
	QRY_COMPLETION_LOCAL_COMMON_LOCATION \
	QRY_COMPLETION_LOCAL_COMMON_OSMDATA \
	QRY_COMPLETION_LOCAL_COMMON_PRODUCT \
	QRY_COMPLETION_LOCAL_COMMON_SOCIAL \
	QRY_COMPLETION_LOCAL_COMMON_NEWS \
	QRY_COMPLETION_LOCAL_COMMON_MOVIE \
	QRY_COMPLETION_LOCAL_COMMON_VIDEO \
	QRY_COMPLETION_LOCAL_COMMON_IMAGE \
; do
${HTTPIE} POST ${TESTURL}/_admin/api/v1/set_simpletemplate \
	name:="\"${name}\"" \
	passkey:="\"${passkey}\"" \
	qtyp:="\"${qtyp}\"" \
	qprof:="`echo ${qprof}`" \
	ignore:="${ignore}"
done

