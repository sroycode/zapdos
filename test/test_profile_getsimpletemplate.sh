#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export qtyp=${qtyp:="QRY_COMPLETION_LOCAL_ONEWORD"};

## ---- main

${HTTPIE} GET ${TESTURL}/_admin/api/v1/get_simpletemplate \
	name=="${name}" \
	qtyp=="${qtyp}"

