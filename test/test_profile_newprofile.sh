#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export newpass=${newpass:="shreos123"};

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/_admin/api/v1/new_profile \
	name:="\"${name}\"" \
	newpass:="\"${newpass}\"" \
	"Shared-Secret:${shared_secret}"
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/_admin/api/v1/new_profile \
	name="${name}" \
	newpass="${newpass}" \
	"Shared-Secret:${shared_secret}"
elif [ ${POSTFORM} -eq 2 ] ; then
${HTTPIE} GET ${TESTURL}/_admin/api/v1/new_profile \
	name=="${name}" \
	newpass=="${newpass}" \
	"Shared-Secret:${shared_secret}"
fi
