#!/bin/sh

#controllo il numero di parametri
if test $# -le 2; then
  echo Numero di parametri sbagliato
  exit 1
fi

#controllo che il primo parametro sia un numero

expr $1 + 0 > /dev/null 2>&1

if test $? -eq 2; then
  echo Solo numeri per favore
  exit 2
fi

#controllo M che sia >0

if test $1 -le 0; then
  echo Solo numeri positivi per favore
  exit 3
fi

tmp=$1
shift

#controllo che $* siano nomi assoluti di direttori
for i
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non direttorio
	    exit 4
	    fi;;
	*)  echo $i non nome assoluto; exit 5;;
	esac
done

>/tmp/tempor

for i
do
    echo fase per $i
    #faccio la chiamata ricorsiva per Gi gerarchia
    es46p.sh $i $tmp /tmp/tempor
done

linee_tot=`wc -l < /tmp/tempor`

echo In totale sono stati creati $linee_tot file

for i in `cat /tmp/ess45`
do
  case $i in
  *.noline) echo empty
            ;;
   *) cat $i
      ;;
  esac
done

rm -rf /tmp/tempor




exit 0



