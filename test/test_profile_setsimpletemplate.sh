#!/bin/bash
export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
. ${SRCDIR}/config.sh

## ---- variables


bash ${SRCDIR}/test_profile_setphotontemplate.sh
bash ${SRCDIR}/test_profile_setnotophtemplate.sh
bash ${SRCDIR}/test_profile_settextdatatemplate.sh
