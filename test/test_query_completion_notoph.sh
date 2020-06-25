#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export profile=${profile:="default"};
export limit=${limit:="10"};
export lon=${lon:="88.88888"};
export lat=${lat:="22.22222"};

export sortd=${sortd:="0"};
export f=${f:="0"};

## ---- main

${HTTPIE} GET ${TESTURL}/_query/api/v1/notoph/${profile} \
	limit=="${limit}" \
	lon=="${lon}" \
	lat=="${lat}"
