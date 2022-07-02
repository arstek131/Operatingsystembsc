#!/bin/sh

#controllo il numero dei parametri
if test $# -le 2; then
  echo Numero paremetri sbagliato
  exit 1
fi

#Controllo che si tratti di un nome relativo semplice
case $1 in
*/*) echo Errore: $1 non in forma relativa semplice
    exit 2;;
*) ;;
esac

tmp=$1
shift
#controllo che si tratti di una directory e che sia assoluta
for i
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory
	    exit 3
	    fi;;
	*)  echo $i non nome assoluto; exit 4;;
	esac
done

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conto$$

for i
do
	echo fase per $i
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i $tmp /tmp/conto$$
done

num_li=`wc -l < /tmp/conto$$`
echo In totale sono stati creati $num_li file

for i in `cat /tmp/conto$$`; do
  echo Il suo nome assoluto $i
  echo La sua prima linea
  head -1 $i | tail -1
  echo La sua ultima linea
  tail -1 $i | head -1
done

rm /tmp/conto$$


exit 0
