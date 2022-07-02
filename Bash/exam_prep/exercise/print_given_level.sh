PATH=`pwd`:$PATH
export PATH

#definiamo una variabile che conterà il livello corrente e la passiamo come terzo parametro al file comandi ricorsivo
conta=0
#il primo livello  (gerarchia G) verra' contato come livello 1: se invece avessimo voluto contarlo come livello 0 allora conta doveva essere inizalizzata a -1

FCR.sh $* $conta


#!/bin/sh
#file comandi ricorsivo dirass K e conta

cd $1

#il primo livello (gerarchia G) verra' contato come livello 1: se invece avessimo voluto contarlo come livello 0 allora conta, nel file comandi principale, doveva essere inizalizzata a -1
#per prima cosa incrementiamo il conteggio del livello
conta=`expr $3 + 1`

#definiamo una variabile per memorizzare i nomi dei file
file=

if test $conta = $2 #siamo al livello giusto e quindi bisogna raccogliere i nomi di tutti i file leggibili
then
echo Siamo al livello giusto dato che conta e\' $conta
for i in *
do
if test -f $i -a -r $i
then
file="$file $i"
fi
done
fi

#se ho trovato almeno un file
if test "$file"
then
echo TROVATO DIRETTORIO `pwd`
echo CHIAMO PARTE C con file $file
15Lug15 $file
fi

for i in *
do
if test -d $i -a -x $i
then
#echo RICORSIONE in `pwd`/$i  con conta uguale a $conta
$0 `pwd`/$i $2 $conta
fi
done
