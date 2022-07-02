#!/bin/sh

#controllo il numero dei parametri

if test $# -lt 2; then
  echo Numero parametri sbagliato
  exit 1
fi

tmp=$1 #mi salvo il parametro Y

shift

#controllo che Y sia un numero

expr $tmp + 0 > /dev/null 2>&1

if test $? -eq 2; then
  echo Solo numeri per favore
  exit 2
fi

#controllo Y che sia >0

if test $tmp -le 0; then
  echo Solo numeri positivi per favore
  exit 3
fi

#ora controllo i $* parametri assicurandomi che siano dir ass.

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

#Il controllo sui parametri è finito vado alle N fasi

export PATH=$(pwd):$PATH

> /tmp/esameDIC #creo il file temporaneo

for i
do
    echo fase per $i
    #faccio la chiamata ricorsiva per Gi gerarchia
    parteR.sh $i $tmp /tmp/esameDIC
done

#Concluse le ricerche ricorsive contono globalmente
#Le linee del file esameDIC
linee=`wc -l < /tmp/esameDIC`

echo Il numero di file globalmente creato e = $linee

#Per ogni file creato in output mostro il nome assoluto del file
#creato, e poi ne mostro anche il contenuto

for i in `cat /tmp/esameDIC`
do
    #Stampo i nomi assoluti dei file trovati
    echo Ho creato il file $i
    #controllo ora il nome del file per vedere se è vuoto o meno
    case $i in
    *NO*) echo il file originale non conteneva almeno 5 linee quindi $i e vuoto
    *) echo il suo contenuto e; cat < $i
    esac
done

#rimuovo il file temporaneo

rm /tmp/esameDIC

















