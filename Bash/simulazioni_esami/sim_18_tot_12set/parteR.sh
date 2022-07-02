#!/bin/sh


#$2 H
#$3 K

cd $1
#chiamata ricorsiva
for fname in *
do
    if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
    fi
done

#controlli del caso
cont=0

for fname in *; do
  if test -f $fname -a -r $fname; then
    cont=`expr $cont + 1`
  fi
done

if test $cont -gt $2 -a $cont -lt $3; then
  echo `pwd`
fi





exit 0
