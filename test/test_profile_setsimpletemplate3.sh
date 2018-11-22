#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"};
export qtyp=${qtyp:="QRY_COMPLETION_NOTOPH"};
if [ -z "${qprof}" ] ; then
export qprof='{
"orders" : [
	{
		"rec_styp": "S_NONE", "rec_tagid": "#t001", "rec_count":5, "desc": "by distance only so band is small",
		"limit_type": "L_NBRHASH5", "search_type": "S_DEFAULT", "order_type": "O_DIST_BAND", "input_type": "I_DEFAULT",
		"distance_band": 10, "weight": 100
	}
]
}';
fi


if [ ${POSTFORM} -eq 0 ] ; then
${HTTPIE} POST ${TESTURL}/_admin/api/v1/set_simpletemplate \
	name:="\"${name}\"" \
	passkey:="\"${passkey}\"" \
	qtyp:="\"${qtyp}\"" \
	qprof:="${qprof}"
fi

