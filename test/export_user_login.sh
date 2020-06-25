export SRCDIR=$(dirname $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
export SRCFIL=$(basename $(cd ${0%/*} 2>>/dev/null ; echo `pwd`/${0##*/}))
sessionkey="`TESTHEADERS=0 bash ${SRCDIR}/test_user_login.sh | jq .sessionkey | sed 's/\"//g'`"
echo "export sessionkey=\"$sessionkey\""
