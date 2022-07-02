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
for fname in *; do
  if test -f $fname -a -r $fname; then
    NL=`wc -l < $fname` #mi salvo tutte le linee del file
      if test $NL -gt 0; then
        NM=`grep '^a' $fname | wc -l`
          if test $NL -eq $NM; then
            echo `pwd`/$fname >> $2
          fi
      fi
  fi

done



exit 0
