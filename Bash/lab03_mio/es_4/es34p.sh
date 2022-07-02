#!/bin/sh

#controllo il numero dei parametri

if [ $# -lt 2 ]; then
  echo "Numero parametri sbagliato"
  exit 1
fi

#controllo che il direttorio sia assoluto

case "$1" in
  /*) ;;
  *) echo "Direttorio assoluto pls"
     exit 2
     ;;
esac

#controllo che si tratti di un direttorio e che sia traversabile

if [ ! -d "$1" -o ! -x "$1" ]; then
 echo "Non si tratta di un direttorio o non e' eseguibile"
 exit 3
fi

export PATH=$(pwd):$PATH
es34r.sh $*

exit 0





