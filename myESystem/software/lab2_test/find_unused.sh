#!/bin/bash
GLOBALS=$(cat fat.c | sed -n '24,67 p')
for g in $GLOBALS; do
	NAME=$(echo $g | sed 's/;$//' | cut -d " " -f 2)
	COUNT=$(cat fat.c | grep "$NAME" | wc -l)
	echo "$NAME: $COUNT"
done
