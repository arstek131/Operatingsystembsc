#!/bin/sh

#$2 F
#$3 tmp file

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
if test -f $2 -a -r $2; then
  sort -f $2 > sorted
  echo `pwd`/sorted >> $3
fi






exit 0
