#!/bin/sh

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i `expr $2 + 1` $3 $4 $5
	fi
done

if test $4 = A; then
  #controlli del caso
  x=`cat $5` #mi salvo il livello attuale
  if test $2 -gt $x; then
    echo $2 > $5
  fi
fi

if test $4 = B; then
  if test $2 -eq $3; then
    pwd
    ls -la
  fi
fi


exit 0
