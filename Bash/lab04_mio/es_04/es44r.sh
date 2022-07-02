#!/bin/sh

cd $1

#chiamata ricorsiva

for fname in *; do
  if test -d $fname -a -x $fname ; then
    $0 $fname $2 $3
  fi
done

#faccio i controlli del caso
for fname in *; do
  if test -f $2 -a -r $fname; then
  >sorted
  sort $2 > sorted
  echo "$(pwd)/sorted"
  echo ok >> $3
  fi
done

exit 0
