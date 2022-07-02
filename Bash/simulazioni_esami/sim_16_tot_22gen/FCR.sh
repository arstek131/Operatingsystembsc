#!/bin/sh

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
cont=0
flag=0
for fname in *; do
  if test -f $fname -a -r $fname; then
    cont=`expr $cont + 1`
  else
    if test -d $fname
      flag=1
    fi
  fi
done

if test $cont -eq $2 -a $flag -eq 0; then
  echo `pwd`
fi


exit 0
