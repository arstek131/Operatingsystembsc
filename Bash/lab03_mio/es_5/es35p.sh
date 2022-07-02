#!/bin/sh

#controllo il numero dei parametri
if [ $# -ne 2 ]; then
  echo "Numero parametri!!"
  exit 1
fi

#controllo che il direttorio sia assoluto
case "$1" in
  /*) ;;
  *) echo "Forma assoluta pls"
     exit 2
     ;;
esac
#controllo che si tratti di un direttorio e che sia traversabile

if [ ! -d "$1" -o ! -x "$1" ]; then
 echo "Passare un direttorio oppure non e' traversabile "
 exit 3
fi

#controllo il 2 parametro che sia un numero e che sia >0

#controllo che si tratti di un numero

expr $2 + 0
if [ $? ! -eq 0 ]; then
  echo "Per favore solo numeri"
  exit 4
fi

#controllo che siano solo numeri >0
if [ $2 -lt 0 ]; then
  echo "Per favore solo numeri positivi"
  exit 5
fi

export PATH=$(pwd):$PATH
es35r.sh $*

exit 0






