#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1
for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2
	fi
done

#controlli del caso
flag=0
num_li=0
for fname in *; do
  if test -f $fname -a -r $fname; then
    num_li=`wc -l < $fname`
    if test $num_li -eq $2; then
      if test $num_li -eq `grep [0-9] $fname | wc -l`; then
        flag=1
        echo `pwd`/$fname
      fi
    fi
  fi
  if test $flag -eq 1; then
    echo `pwd`
  fi
done



exit 0
