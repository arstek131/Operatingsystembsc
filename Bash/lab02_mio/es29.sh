#!/bin/bash

if [ $# -ne 1 -o ! -d $1 ]; then
  echo "errore"
  exit 1
fi

dire=0
files=0

for fname in "$1"/*; do

  if [ -d "$fname" ]; then
    echo D "$fname"
    dire=`expr $dire + 1`
  fi

  if [ -f "$fname" ]; then
    echo F "$fname"
    files=`expr $files + 1`
  fi

done

echo "Number of dir " $dire
echo "Number of files " $files

exit 0


