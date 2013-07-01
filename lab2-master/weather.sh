#!/bin/bash
i=`cat config.ini`

if [ "$1" = "--help" ]
then
echo I print weather every $i seconds.
exit 0
fi


url="pda.pogoda.by";


if ping -c1 $url >/dev/null
then
	while true  
	do  	
		wget -q -O- $url | iconv -f cp1251 -t utf8>tmp.txt>> tmp.txt
		weather=`cat ./tmp.txt | grep -A 1 "погода фактическая"| sed 's/<br>/\n/g'|sed 's/<[^>]*>/ /g'`
		echo "$weather"
		echo
		echo
		rm tmp.txt
    		sleep $i
	done
else
	echo "no connection"
fi


