#!/bin/bash
#
#
#

FILE_LIST=`ls results`
if [ -z "${FILE_LIST}" ]
then
echo "Run \"make run\" first."
else
for tld in $*
do
count=0
for file in ${FILE_LIST}
do
let "count = count + "`grep -c ${tld} results/${file}`""
done
if [ $# -gt 1 ] 
then 
echo ${tld} ${count}
else 
echo ${count}
fi
done
fi

exit 0