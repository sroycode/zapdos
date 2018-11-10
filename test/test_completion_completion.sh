#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables
export scope=${scope:="local"};
export profile=${profile:="shreos"};
export q=${q:="restaurants in koraman"};
export f=${f:="0"};
export ps=${ps:="10"};
export sc=${sc:="IND"};
export ss=${ss:=""};
export st=${st:="bangalore"};
export sd=${st:="bangalore"};
export sa=${sa:="koramangala"};
export sx=${sx:="77.61132"};
export sy=${sy:="12.93605"};
export sz=${sz:="560095"};
export sp=${sp:="0"};

export dc=${dc:="IND"};
export dd=${dd:=""};
export dt=${dt:="bangalore"};
export dd=${dt:="bangalore"};
export da=${da:="koramangala"};
export dx=${dx:="77.61132"};
export dy=${dy:="12.93605"};
export dz=${dz:="560095"};
export dp=${dp:="1"};

## ---- main

${HTTPIE} GET ${TESTURL}/_query/api/v1/completion/${scope}/${profile} \
	q=="${q}" \
	f=="${f}" \
	ps=="${ps}" \
	sc=="${sc}" \
	ss=="${ss}" \
	st=="${st}" \
	sd=="${sd}" \
	sa=="${sa}" \
	sx=="${sx}" \
	sy=="${sy}" \
	sz=="${sz}" \
	sp=="${sp}" \
	dc=="${dc}" \
	ds=="${ds}" \
	dt=="${dt}" \
	dd=="${dd}" \
	da=="${da}" \
	dx=="${dx}" \
	dy=="${dy}" \
	dz=="${dz}" \
	dp=="${dp}"
