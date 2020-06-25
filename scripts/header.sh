#!/bin/sh
if [ $# -lt 1 ] ; then echo "Usage: $0 FIL" ; exit 1 ; fi
DIR="`dirname $1`"
FIL="`basename $1`"
COMMENTS=$2

awk -v DIR="${DIR}" -v FIL="${FIL}" -v COMMENTS="${COMMENTS}" '
BEGIN{FS="\:";act=0; FLINE=" *  "FIL" : "COMMENTS; }
act==0 && $1 ~ FIL && $2~/[A-z]/ { FLINE=$0}
act==0 && $0 ~ /^#/ {
act=1;
print "/**";
print " * @project zapdos";
print " * @file "DIR"/"FIL;
print " * @author  S Roychowdhury < sroycode at gmail dot com >"
print " * @version 1.0.0";
print " *";
print " * @section LICENSE";
print " *";
print " * Copyright (c) 2018-2020 S Roychowdhury";
print " *";
print " * Permission is hereby granted, free of charge, to any person obtaining a copy of";
print " * this software and associated documentation files (the \042Software\042), to deal in";
print " * the Software without restriction, including without limitation the rights to";
print " * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of";
print " * the Software, and to permit persons to whom the Software is furnished to do so,";
print " * subject to the following conditions:";
print " *";
print " * The above copyright notice and this permission notice shall be included in all";
print " * copies or substantial portions of the Software.";
print " *";
print " * THE SOFTWARE IS PROVIDED \042AS IS\042, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR";
print " * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS";
print " * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR";
print " * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER";
print " * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN";
print " * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";
print " *";
print " * @section DESCRIPTION";
print " *";
print FLINE;
print " *";
print " */";
}
act==1{
print $0
}' $1 > ${1}.temp
mv ${1}.temp $1
