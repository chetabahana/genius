#!/bin/sh

echo Running xmlto -o C/html/ html C/genius.xml
xmlto -o C/html/ html C/genius.xml || exit 1

echo Running docbook2txt C/genius.xml
docbook2txt C/genius.xml || exit 1
echo Running dos2unix genius.txt
dos2unix genius.txt || exit 1

LANGS="cs de el es fr pt_BR ru sv"
for n in $LANGS ; do
	echo Running xml2po -e -p $n/$n.po -o $n/genius.xml C/genius.xml
	xml2po -e -p $n/$n.po -o $n/genius.xml C/genius.xml || exit 1
	echo Running xmlto -o $n/html/ html $n/genius.xml
	xmlto -o $n/html/ html $n/genius.xml || exit 1
done
