#!/bin/sh

#controllo il numero dei parametri
if [ $# -ne 2 ]; then
  echo "Consentiti 2 parametri"
  exit 1
fi

#controllo che il primo parametro sia dato in modo  assoluto

case "$1" in
  /*) ;;
  *) echo "Nome assoluto per favore"
     exit 2
     ;;

#controllo che il primo parametro sia un direttorio e che sia travers.

if [ ! -d "$1" -o ! -x "$1" ]; then
    echo "Non e' un direttorio o non e' traversabile"
    exit 3
fi

#esporto il path

export PATH=$(pwd):$PATH

#chiamo il file ricorsivo passandoci tutti i parametri
es32r.sh $*

exit 0







