#!/bin/sh

#$2 CZ carattere da cercare
#$3 /tmp/

#chiamata ricorsiva
for fname in *
do
    if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
    fi
done

#controlli del caso

for fname in *; do
  if test -f $fname -a -r $fname; then
    grep $2 $fname > /dev/null 2>&1
    if test $? -eq 0; then
      echo "Trovato il file `pwd`/$fname"
      echo `pwd`/$fname >> $3
    fi
  fi
done



exit 0
