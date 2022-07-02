#!/bin/sh

cd "$1"

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    $0 "$fname" "$2"
  fi
done

#eseguo i contorlli del caso

j=0
i=0
n_lines=0
for fname in *; do
  if [ -f "$fname" -a -r "$fname" ]; then
    i=`expr $i + 1`
    n_lines=$(wc -l "$fname")
        if [ "$n_lines" -eq "$2" ]; then
             j=`expr $j + 1`
        fi
  fi
done

if [ "$j" -eq "$i" ]; then
  echo $(pwd)
fi

exit 0



