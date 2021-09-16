#!/bin/sh

g++ egsv2.cpp -lcurl
./a.out list2.txt
awk '{print $2" "$3}' list2.txt > list3.txt
perl -pe 's|https.*?\/comic.*?\/||g' list3.txt > list4.txt
# first sed deletes space-only lines (blank lines / last line)
# second sed puts ending javascript on the end
cat comicdatahead.txt list4.txt | sed '/^ *$/d' | sed '$ s/$/`.split("\\n");/' > comicdata.js

rm list3.txt
rm list4.txt
rm a.out
