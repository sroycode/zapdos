#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables

export name=${name:="shreos"};
export passkey=${passkey:="shreos123"};
export qtyp=${qtyp:="QRY_COMPLETION_TEXTDATA"};
if [ -z "${qprof}" ] ; then
export qprof='{
"orders" : [
	{
		"rec_styp": "S_NONE", "rec_tagid": "#t001", "rec_count":10, "desc": "one word query for upto 2 chars",
		"limit_type": "L_NONE", "search_type": "S_BEGINWITH", "order_type": "O_DEFAULT",
		"input_type": "I_ONEWORD", "oneword_length" : 2, "weight": 100
	},
	{
		"rec_styp": "S_NONE", "rec_tagid": "#t002", "rec_count":10, "desc": "find in name",
		"limit_type": "L_NONE", "search_type": "S_NAME", "order_type": "O_DEFAULT",
		"input_type": "I_DEFAULT", "weight": 90
	},
	{
		"rec_styp": "S_NONE", "rec_tagid": "#t003", "rec_count":10, "desc": "if not name",
		"limit_type": "L_NONE", "search_type": "S_DEFAULT", "order_type": "O_DEFAULT",
		"input_type": "I_DEFAULT", "weight": 80
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

