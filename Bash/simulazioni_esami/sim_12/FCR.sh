#!/bin/sh

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 `expr $3 + 1` $4
	fi
done

if test $2 = 'A'; then
  echo $3 >> $4
fi





exit 0
