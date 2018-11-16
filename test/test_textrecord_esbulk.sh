#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export index=${index:="myindex"};
export type=${type:="mytype"};
export styp=${rtyp:="S_NONE"};
export lang=${lang:="en"};
export last_up=${last_up:="1530076889241"};
export last_id=${last_id:="0"};
export limit=${limit:="10"};
## ---- main

${HTTPIE} GET ${TESTURL}/_data/api/v1/textrecord/esbulk \
	index=="${index}" \
	type=="${type}" \
	rtyp=="${rtyp}" \
	lang=="${lang}" \
	last_up=="${last_up}" \
	last_id=="${last_id}" \
	limit=="${limit}"
