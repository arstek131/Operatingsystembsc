#!/bin/sh
#FCR.sh
#file comandi ricorsivo che scrive il nome dei direttori trovati sul file temporaneo
#il cui nome e' passato come terzo argomento

cd $1 #ci posizioniamo nella directory giusta
NL= 	#variabile in cui salviamo il numero di linee del file corrente


for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 $3
	fi
done

#controlli del caso
case $1 in
*/$2)
	for i in *
	do
		if test -f $i -a -r $i
		then
			NL=`wc -l < $i`
			if test $NL -ge 5
			then
				tail -5 $i | head -1 > $i.quintultima
				echo `pwd`/$i.quintultima >> $3
			else
				> $i.NOquintultima
				echo `pwd`/$i.NOquintultima >> $3
			fi
		fi
	done;;
*);;
esac



exit 0
