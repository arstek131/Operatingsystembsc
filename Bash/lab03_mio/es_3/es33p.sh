#!/bin/sh

#controllo il numero dei parametri

if [ $# -ne 3 ]; then
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
#controllo che sia un direttorio e che sia traversabile

if [ ! -d "$1" -o ! -x "$1" ]; then
  echo "Non e' un direttorio o non è traversabile"
  exit 3
fi

#faccio il controllo che siano singoli caratteri numerici $2 e $3

#controllo che siano dei numeri entrambi

expr $2 + 0
if [ $? ! -eq 0 ]; then
    echo "Per favore solo numeri"
    exit 4
fi


expr $3 + 0
if [ $? ! -eq 0 ]; then
    echo "Per favore solo numeri"
    exit 4
fi

#controllo che siano costituiti da sola una cifra

case "$2" in
  [0-9]) ;;
  *) echo "Max una cifra pls"
    exit 5
    ;;
esac


case "$3" in
  [0-9]) ;;
  *) echo "Max una cifra pls"
    exit 5
    ;;
esac

export PATH=$(pwd):$PATH
es33r.sh $*

exit 0


