#!/bin/sh

#controllo numero di parametri

if test $# -ne 2; then
  echo Numero parametri sbagliato
  exit 1
fi

#controllo che il primo parametro abbia nome assoluto
case "$1" in
  /*) ;;
  *) echo Forma assoluta pls
     exit 2
     ;;
esac

#controllo che si tratti di una directory e che sia traversabile

if test ! -d $1 -o ! -x $1; then
  echo "Non si tratta di una directory oppure non e' traversabile"
  exit 3
fi

#controllo che il secondo parametro abbia nome relativo semplice

case "$2" in
  */*) echo Nome relativo semplice pls
       exit 4
       ;;
  *) ;; #significa che è relativo semplice
esac

export PATH=$(pwd):$PATH
es43r.sh $*

contatore=0

if test -f "/tmp/lista"; then
    contatore=$(wc -l < /tmp/lista)
fi

echo "In totale sono stati creati $contatore direttori"

exit 0



