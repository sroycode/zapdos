#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export username=${username:="admin"};
export password=${password:="admin123"};
export password=${default_adminpass}

## ---- main

if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/_admin/api/v1/login \
	username:="\"${username}\"" \
	password:="\"${password}\""
elif [ ${POSTFORM} -eq 1 ] ; then
${HTTPIE} --form POST ${TESTURL}/_admin/api/v1/login \
	username="${username}" \
	password="${password}"
fi
