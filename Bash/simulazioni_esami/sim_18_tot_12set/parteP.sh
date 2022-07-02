#!/bin/sh

#controllo il numero di parametri
if test $# -le 3; then
  echo Numero parametri sbagliato
  exit 1
fi

#controllo che il primo parametro sia un numero
expr $1 + 0 > /dev/null 2>&1

if test ! $? -eq 0; then
  echo Solo numeri per favore
  exit 2
fi

#controllo che sia >0
if test $1 -le 0; then
  echo Solo numeri positivi per favore
  exit 4
fi

#controllo che il secondo parametro sia un numero
expr $2 + 0 > /dev/null 2>&1

if test ! $? -eq 0; then
  echo Solo numeri per favore
  exit 5
fi

#controllo che sia >0
if test $2 -le 0; then
  echo Solo numeri positivi per favore
  exit 6
fi

tmp1=$1
shift
tmp2=$1
shift

#controllo che i parametri rimanenti siano nomi assoluti di directory
for i
do
    case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non direttorio
	    exit 7
	    fi;;
	*)  echo $i non nome assoluto; exit 7;;
	esac
done

#ho finito il controllo sui parametri mi preparo a lanciare le N fasi

export PATH=$(pwd):$PATH

for i
do
  parteR.sh $i $tmp1 $tmp2
done



exit 0
