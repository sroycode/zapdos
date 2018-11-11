#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"};
export qtyp=${qtyp:="QRY_COMPLETION_PHOTON"};
if [ -z "${qprof}" ] ; then
export qprof='{
"orders" : [
	{
		"rec_styp": "S_NONE", "rec_tagid": "#t001", "rec_count":10, "desc": "places to add",
		"limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DIST_BAND", "input_type": "I_QUERY",
		"distance_band": 1000, "weight": 10
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

