#!/bin/sh
awk '{gsub(/^.{0,32}/,"",$2);gsub(/^.{0,33}/,"",$3);print$2" "$3}' list2.txt > list4.txt
cat comicdatahead.txt list4.txt | sed '/^ *$/d' | sed '$ s/$/`.split("\\n");/' > comicdata.js
