#!/bin/sh

cd "$1"

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    $0 "$fname" "$2"
  fi
done

#eseguo i controlli del caso
i=0
tmp=null
for fname in *; do
  if [ -f "$fname" ]; then
  i=`expr $i + 1`
  tmp="$fname"
  fi
done

if [ "$i" -ge 1 -a "$i" -le "$2" ]; then
  echo $(pwd) "$tmp"
fi

exit 0







