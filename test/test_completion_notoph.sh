#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export lang=${lang:="en"};
export profile=${profile:="shreos"};
export limit=${limit:="10"};
export lon=${lon:="77.61132"};
export lat=${lat:="12.93605"};

export sortd=${sortd:="0"};
export f=${f:="0"};

## ---- main

${HTTPIE} GET ${TESTURL}/_query/api/v1/notoph/${profile} \
	lang=="${lang}" \
	limit=="${limit}" \
	lon=="${lon}" \
	lat=="${lat}"
