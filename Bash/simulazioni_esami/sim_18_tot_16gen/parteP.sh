#!/bin/sh

#controllo il numero dei parametri
if test -ne 2; then
  echo Numero parametri sbagliato
  exit 1
fi

#Controllo primo parametro sia dato in forma assoluta
case $1 in
/*) if test ! -d $1 -o ! -x $1
	    then
	    echo $1 non directory
	    exit 2
	    fi;;
*)  echo $1 non nome assoluto; exit 3;;
esac

#controllo che F sia un nome relativo semplice
case $2 in
*/*) echo Errore: $2 non in forma relativa semplice
     exit 4;;
*) ;;
esac

#controlli sui parametri finiti
PATH=`pwd`:$PATH
export PATH

parteR.sh $1 $2


exit 0
