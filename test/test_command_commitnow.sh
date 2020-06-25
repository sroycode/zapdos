#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

## ---- main

${HTTPIE} GET ${TESTURL}/_admin/api/v1/local_command/commitnow \
	"Shared-Secret:${shared_secret}"

