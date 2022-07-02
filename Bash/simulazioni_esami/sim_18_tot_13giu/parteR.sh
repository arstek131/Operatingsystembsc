#!/bin/sh

#$2 D relative path to check
#$3 Y numbers of line to check
#$4 temp file /tmp/


#chiamata ricorsiva
for fname in *
do
    if test -d $fname -a -x $fname; then
    $0 $fname $2 $3 $4
    fi
done

flag=0
#controlli del caso
case $1 in
  */$2) for fname in *; do
        if test -f $fname -a -r $fname; then
          linee=`wc -l < $fname`
          if test $linee -eq $3; then
            flag=1
            echo cacca >> $4
          fi
        fi
        done
        if test $flag -eq 1; then
          echo `pwd`/$2
        fi
      ;;
  *) ;;
esac



exit 0
