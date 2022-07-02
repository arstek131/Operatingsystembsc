#!/bin/sh

cd $1

#chiamata ricorsiva
for i in *; do
  if test -d $fname -a -x $fname; then
    $0 $i $2 $3
  fi
done

#controlli del caso
flag1=0
flag2=0
tmp_line1=0
tmp_line2=0
tmp_car1=0
tmp_car2=0
for fname in *; do
 if test -f $fname; then
  case $fname in
    $2.1) flag1=1
          tmp_line1=`wc -l < $fname`
          tmp_car1=`wc -m < $fname`
          ;;
    $2.2) flag2=1
          tmp_line2=`wc -l < $fname`
          tmp_car2=`wc -m < $fname`
          ;;
    *) ;;
  esac
 fi
done

if test $flag1 -eq 1 -a $flag2 -eq 1; then
  if test $tmp_line1 -eq $tmp_line2; then
    if test $tmp_car1 -eq $tmp_car2; then
      echo `pwd`
    fi
  fi
fi





exit 0
