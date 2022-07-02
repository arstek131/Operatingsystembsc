#!/bin/sh

cd $1

for fname in *; do
  if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
  fi
done


#controlli del caso
bytes=0
flag=0
for fname in *; do
  if test -f $fname -a -r $fname -a -w $fname; then
    bytes=`wc -c < $fname`
    grep $3 $fname > /dev/null 2>&1
    if test $? -eq 0 -a $bytes -eq $2; then
      flag=1
    fi
  fi
done

if test $flag -eq 1; then
  echo `pwd`
fi









exit 0
