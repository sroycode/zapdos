#!/bin/bash;

if [ ! "`which http`" -o ! "`which jq`" ] ; then
	echo "httpie or jq not installed";
	exit 1;
fi

if [ -z "${SRCDIR}" -o -z "${SRCFIL}" ] ; then
	echo "define SRCDIR and SRCFIL in test program";
	exit 1;
fi


# if testbatch is not set , set 0
export TESTBATCH=${TESTBATCH:=0};

# if testheaders is not set , set 0
export TESTHEADERS=${TESTHEADERS:=0};

# test directory
export TESTDIR=${TESTDIR:="./testdir"};

HTTPIE="http"
if [ ${TESTBATCH} -eq 1 ] ; then
	if [ ! -d ${TESTDIR} ] ; then mkdir ${TESTDIR} ; fi
	HTTPIE="${HTTPIE} -o ${TESTDIR}/${SRCFIL}.out";
elif [ ${TESTHEADERS} -eq 1 ] ; then
	HTTPIE="${HTTPIE} --print BHbh";
fi
export HTTPIE

# if postform is not set , set 0 i.e. posts are json
export POSTFORM=${POSTFORM:=0};

export TESTURL=${TESTURL:="http://localhost:9093"};

## shared_secret dummy
export shared_secret=${shared_secret:="please-change-this-secret"};
export default_adminpass=${default_adminpass:="please-change-this-password"};

