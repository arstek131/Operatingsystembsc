#!/bin/sh

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 $3 $4 $5
	fi
done

#controlli del caso
if test $4 = A; then
  if test -f $2; then
    pwd
    echo `pwd`/$2 >> $5
  fi
fi

if test $4 = B; then
  if test ! -f $2; then
    pwd
    > $2
  fi
fi



exit 0
