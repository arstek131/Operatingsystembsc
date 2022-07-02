#!/bin/sh

cd $1


for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 $3
	fi
done

#controlli del caso
for fname in *; do
  if test -f $fname -a -r $fname; then
    NL=`wc -l < $fname`
    NG=`grep 't$' $fname | wc -l`
    if test `grep 't$' $fname | wc -l` -ge $2; then
      echo `pwd`/$fname >> $3
    fi
  fi
done





exit 0
