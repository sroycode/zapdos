#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"};

## ---- main

${HTTPIE} GET ${TESTURL}/_admin/api/v1/check_simpletemplates \
	name=="${name}" \
	passkey=="${passkey}" 

