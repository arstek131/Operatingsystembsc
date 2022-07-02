#!/bin/sh

#file comandi comincia.sh

d=      #variabile usata per memorizzare o il primo parametro oppure la directory corrente

f= # variabile usata per memorizzara il parametro che rappresenta il nome relativo semplice del file (primo o secndo parametro a seconda dei casi)

case $# in

0) echo "POCHI PARAMETRI - Usage is: $0 [direttorio] file"
    exit 1;;

1) d=`pwd` ; f=$1;;

2) d=$1; f=$2;;

*) echo "TROPPI PARAMETRI - Usage is: $0 [direttorio] file"
    exit 2;;

esac

#controllo che il primo parametro sia dato in forma assoluta

case $d in
/*) ;;

*) echo Errore: $d non in forma assoluta
    exit 3;;
esac

#controllo che il secondo parametro sia dato in forma relativa semplice

case $f in
*/*) echo Errore: $f non in forma relativa semplice
    exit 4;;
*) ;;
esac

#controllo che il primo parametro sia un direttorio e che sia accessibile

if [ ! -d $d -o -x $d ]
  then echo Errore: $d non direttorio; exit 5
fi

PATH=`pwd`:$PATH

export PATH

echo comincia.sh: Stiamo per esplorare il direttorio $d
cerca.sh $d $f
echo HO FINITO TUTTO
